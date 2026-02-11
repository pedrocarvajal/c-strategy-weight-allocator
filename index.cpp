#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

#include "structs/strategy_snapshot.hpp"
#include "structs/symbol_snapshot.hpp"
#include "structs/performance.hpp"
#include "services/logger/headers/logger.hpp"

#include "helpers/headers/get_performance.hpp"

#include "libraries/json.hpp"

int main() {
    namespace fs = std::filesystem;
    using json = nlohmann::json;
    using StrategySnapshot = structs::StrategySnapshot;
    using MarketSnapshot = structs::MarketSnapshot;
    using Performance = structs::Performance;

    services::Logger logger("index");

    std::regex pattern(".*_Snapshots\\.json$");
    std::vector<json> files;
    std::map<int, std::vector<StrategySnapshot> > strategy_snapshots;
    std::map<int, std::vector<MarketSnapshot> > symbol_snapshots;

    for (auto& entry : fs::directory_iterator("storage/")) {
        std::string filename = entry.path().filename().string();

        if (std::regex_match(filename, pattern)) {
            std::ifstream file(entry.path());
            json data = json::parse(file);
            std::string level = data[0].value("level", "");
            long prev_timestamp = 0;

            for (auto& snapshot: data) {
                long timestamp = snapshot["timestamp"];
                std::time_t time = static_cast<std::time_t>(timestamp);
                std::tm *datetime = std::localtime(&time);
                std::string symbol = snapshot.value("symbol", "");

                if (level == "symbol") {
                    double bid = snapshot.value("bid", 0.0);
                    double ask = snapshot.value("ask", 0.0);
                    double daily_performance = 0;

                    if (!symbol_snapshots.empty()) {
                        auto iterator = symbol_snapshots.find(prev_timestamp);

                        if (iterator != symbol_snapshots.end()) {
                            for (auto& prev_snap: iterator->second) {
                                if (prev_snap.symbol == symbol) {
                                    Performance p = helpers::get_performance(ask, prev_snap.ask);
                                    daily_performance = p.percentage;
                                    break;
                                }
                            }
                        }
                    }

                    MarketSnapshot market_snapshot;
                    market_snapshot.datetime = *datetime;
                    market_snapshot.symbol = symbol;
                    market_snapshot.bid = bid;
                    market_snapshot.ask = ask;
                    market_snapshot.daily_performance = daily_performance;

                    logger.info(std::to_string(market_snapshot.daily_performance));

                    symbol_snapshots[timestamp].push_back(market_snapshot);
                } else if (level == "strategy") {
                    std::string strategy_symbol = snapshot.value("strategy_symbol", "");
                    std::string strategy_name = snapshot.value("strategy_name", "");
                    std::string strategy_prefix = snapshot.value("strategy_prefix", "");

                    double nav = snapshot.value("nav", 0.0);
                    double performance = snapshot.value("performance", 0.0);
                    double daily_performance = 0;

                    if (!strategy_snapshots.empty()) {
                        auto iterator = strategy_snapshots.find(prev_timestamp);

                        if (iterator != strategy_snapshots.end()) {
                            for (auto& prev_snap: iterator->second) {
                                if (prev_snap.strategy_name == strategy_name &&
                                    prev_snap.strategy_symbol == strategy_symbol) {
                                    daily_performance = (performance - prev_snap.performance) / prev_snap.performance;

                                    Performance p = helpers::get_performance(performance, prev_snap.performance);
                                    daily_performance = p.percentage;
                                    break;
                                }
                            }
                        }
                    }

                    StrategySnapshot strategy_snapshot;
                    strategy_snapshot.datetime = *datetime;
                    strategy_snapshot.strategy_symbol = strategy_symbol;
                    strategy_snapshot.strategy_name = strategy_name;
                    strategy_snapshot.strategy_prefix = strategy_prefix;
                    strategy_snapshot.nav = nav;
                    strategy_snapshot.performance = performance;
                    strategy_snapshot.daily_performance = daily_performance;

                    strategy_snapshots[timestamp].push_back(strategy_snapshot);
                }

                prev_timestamp = timestamp;
            }
        }
    }
}
