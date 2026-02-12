#include <ctime>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <map>
#include <set>

#include "structs/strategy_snapshot.hpp"
#include "structs/market_snapshot.hpp"

#include "services/logger/headers/logger.hpp"

#include "helpers/headers/get_market_snapshots.hpp"
#include "helpers/headers/get_strategy_snapshots.hpp"
#include "helpers/headers/normalize_features.hpp"
#include "helpers/headers/build_strategy_lookup.hpp"
#include "helpers/headers/date_key_from_datetime.hpp"

#include "libraries/json.hpp"

int main() {
    namespace fs = std::filesystem;
    services::Logger logger("index");

    // ---------------------------------------------------------------
    // Config
    // ---------------------------------------------------------------
    const int NORMALIZATION_WINDOW = 365;
    const int K_NEIGHBORS = 35;
    const int MAX_ACTIVE_STRATEGIES = 10;
    const double EPSILON = 0.0001;

    // ---------------------------------------------------------------
    // 1. Load snapshots
    // ---------------------------------------------------------------
    std::regex pattern(".*_Snapshots\\.json$");
    std::vector<structs::MarketSnapshot> market_snapshots;
    std::vector<structs::StrategySnapshot> strategy_snapshots;
    std::vector<int> timestamps;

    for (auto& entry : fs::directory_iterator("storage/")) {
        std::string filename = entry.path().filename().string();

        if (std::regex_match(filename, pattern)) {
            std::ifstream file(entry.path());
            nlohmann::json data = nlohmann::json::parse(file);
            std::string level = data[0].value("level", "");

            if (level == "market") {
                helpers::get_market_snapshots(data, market_snapshots, timestamps);
            } else if (level == "strategy") {
                helpers::get_strategy_snapshots(data, strategy_snapshots);
            } else {
                throw std::runtime_error(
                          "Unrecognized snapshot level '" + level
                          + "' in file: " + filename
                          + ". Expected 'market' or 'strategy'."
                );
            }
        }
    }

    if (market_snapshots.empty()) {
        throw std::runtime_error(
                  "No market snapshots loaded."
                  " Verify that 'storage/' contains '*_Snapshots.json' files with level 'market'."
        );
    }

    if (strategy_snapshots.empty()) {
        throw std::runtime_error(
                  "No strategy snapshots loaded."
                  " Verify that 'storage/' contains '*_Snapshots.json' files with level 'strategy'."
        );
    }

    // ---------------------------------------------------------------
    // 2. Build feature vectors and normalize (rolling z-score)
    // ---------------------------------------------------------------
    std::vector<std::vector<double> > features;
    std::vector<std::vector<double> > features_normalized;

    helpers::normalize_features(
        market_snapshots, timestamps, NORMALIZATION_WINDOW,
        features, features_normalized
    );

    // ---------------------------------------------------------------
    // 3. Build strategy lookup (date -> strategy -> daily_performance)
    // ---------------------------------------------------------------
    std::map<int, std::map<std::string, double> > strategy_by_date;
    std::set<std::string> strategy_names;

    helpers::build_strategy_lookup(strategy_snapshots, strategy_by_date, strategy_names);

    logger.info("market days: " + std::to_string(market_snapshots.size())
                + " | normalized days: " + std::to_string(features_normalized.size())
                + " | strategies: " + std::to_string(strategy_names.size()));

    // ---------------------------------------------------------------
    // 4. KNN semaphore: for each day, find similar past conditions
    //    and activate strategies that performed well under them.
    //    Decision on day T, return measured on day T+1.
    // ---------------------------------------------------------------
    double nav_raw = 1.0;
    double nav_knn = 1.0;
    double nav_raw_peak = 1.0;
    double nav_knn_peak = 1.0;
    double max_dd_raw = 0.0;
    double max_dd_knn = 0.0;
    int days_active_zero = 0;
    int total_days = 0;

    for (size_t today_index = 1; today_index + 1 < features_normalized.size(); ++today_index) {
        const auto& today = features_normalized[today_index];

        // 4a. Euclidean distance to all past normalized days
        std::vector<std::pair<double, size_t> > distances;
        for (size_t j = 0; j < today_index; ++j) {
            double sum_squared = 0.0;
            for (size_t f = 0; f < today.size(); ++f) {
                double diff = today[f] - features_normalized[j][f];
                sum_squared += diff * diff;
            }
            distances.push_back({ std::sqrt(sum_squared), j });
        }

        // 4b. Select K nearest neighbors
        int k = std::min(K_NEIGHBORS, static_cast<int>(distances.size()));
        std::sort(distances.begin(), distances.end());
        std::vector<std::pair<double, size_t> > nearest_neighbors(
            distances.begin(),
            distances.begin() + k
        );

        // 4c. Score each strategy (inverse-distance weighted average)
        double weight_sum = 0.0;
        for (const auto& [dist, norm_index] : nearest_neighbors)
            weight_sum += 1.0 / (dist + EPSILON);

        std::map<std::string, double> strategy_scores;
        for (const auto& name : strategy_names) {
            double weighted_sum = 0.0;

            for (const auto& [dist, norm_index] : nearest_neighbors) {
                size_t original_index = norm_index + NORMALIZATION_WINDOW;
                const auto& market_snap = market_snapshots[original_index];
                int date_key = helpers::date_key_from_datetime(market_snap.datetime);
                double weight = 1.0 / (dist + EPSILON);

                auto date_it = strategy_by_date.find(date_key);
                if (date_it != strategy_by_date.end()) {
                    auto strat_it = date_it->second.find(name);
                    if (strat_it != date_it->second.end())
                        weighted_sum += weight * strat_it->second;
                }
            }

            strategy_scores[name] = weighted_sum / weight_sum;
        }

        // 4d. Activate top strategies with positive score
        std::vector<std::pair<double, std::string> > sorted_scores;
        for (const auto& [name, score] : strategy_scores)
            if (score > 0.0)
                sorted_scores.push_back({ score, name });

        std::sort(sorted_scores.rbegin(), sorted_scores.rend());

        int active_count = std::min(MAX_ACTIVE_STRATEGIES, static_cast<int>(sorted_scores.size()));
        std::vector<std::pair<double, std::string> > active_strategies(
            sorted_scores.begin(),
            sorted_scores.begin() + active_count
        );

        // ---------------------------------------------------------------
        // 5. Measure performance on T+1 (no look-ahead)
        // ---------------------------------------------------------------
        size_t original_today = today_index + NORMALIZATION_WINDOW;
        size_t next_day = original_today + 1;
        const auto& next_snap = market_snapshots[next_day];
        int next_date_key = helpers::date_key_from_datetime(next_snap.datetime);
        auto next_it = strategy_by_date.find(next_date_key);

        double raw_return = 0.0;
        if (next_it != strategy_by_date.end()) {
            for (const auto& [name, perf] : next_it->second)
                raw_return += perf;
            raw_return /= static_cast<double>(next_it->second.size());
        }
        nav_raw *= (1.0 + raw_return);

        double knn_return = 0.0;
        if (active_count > 0 && next_it != strategy_by_date.end()) {
            for (const auto& [score, name] : active_strategies) {
                auto strat_it = next_it->second.find(name);
                if (strat_it != next_it->second.end())
                    knn_return += strat_it->second;
            }
            knn_return /= static_cast<double>(active_count);
        }
        nav_knn *= (1.0 + knn_return);

        // ---------------------------------------------------------------
        // 6. Track drawdown
        // ---------------------------------------------------------------
        nav_raw_peak = std::max(nav_raw_peak, nav_raw);
        nav_knn_peak = std::max(nav_knn_peak, nav_knn);
        max_dd_raw = std::min(max_dd_raw, (nav_raw - nav_raw_peak) / nav_raw_peak);
        max_dd_knn = std::min(max_dd_knn, (nav_knn - nav_knn_peak) / nav_knn_peak);

        if (active_count == 0) days_active_zero++;
        total_days++;

        // ---------------------------------------------------------------
        // 7. Daily log
        // ---------------------------------------------------------------
        char date_buffer[11];
        std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", &next_snap.datetime);

        char nav_buffer[128];
        std::snprintf(nav_buffer, sizeof(nav_buffer),
                      "raw: %.4f (dd: %.2f%%) | knn: %.4f (dd: %.2f%%) | active: %d",
                      nav_raw, max_dd_raw * 100, nav_knn, max_dd_knn * 100, active_count);
        logger.info(std::string(date_buffer) + " | " + nav_buffer);
    }

    // ---------------------------------------------------------------
    // 8. Summary
    // ---------------------------------------------------------------
    logger.info("=== SUMMARY ===");
    logger.info("total days: " + std::to_string(total_days)
                + " | days inactive: " + std::to_string(days_active_zero));

    char summary[256];
    std::snprintf(summary, sizeof(summary),
                  "RAW  -> nav: %.4f | max_dd: %.2f%%", nav_raw, max_dd_raw * 100);
    logger.info(summary);

    std::snprintf(summary, sizeof(summary),
                  "KNN  -> nav: %.4f | max_dd: %.2f%% | ratio: %.2fx",
                  nav_knn, max_dd_knn * 100, nav_knn / nav_raw);
    logger.info(summary);
}
