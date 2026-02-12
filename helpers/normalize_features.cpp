#include "helpers/headers/normalize_features.hpp"

#include <cmath>

#include "libraries/Eigen/Dense"

namespace helpers {
    void normalize_features(
        const std::vector<structs::MarketSnapshot> &market_snapshots,
        const std::vector<int> &timestamps,
        int normalization_window,
        std::vector<std::vector<double>> &features,
        std::vector<std::vector<double>> &features_normalized
    ) {
        for (size_t i = 0; i < timestamps.size(); ++i) {
            const auto& snapshot = market_snapshots[i];

            features.push_back({
                snapshot.performance,
                snapshot.volatility,
                snapshot.drawdown,
            });

            if (i < static_cast<size_t>(normalization_window))
                continue;

            std::vector<double> list_of_performance;
            std::vector<double> list_of_volatility;
            std::vector<double> list_of_drawdown;

            for (size_t y = (i - normalization_window); y < i; ++y) {
                list_of_performance.push_back(features[y][0]);
                list_of_volatility.push_back(features[y][1]);
                list_of_drawdown.push_back(features[y][2]);
            }

            Eigen::Map<Eigen::VectorXd> e_performance(list_of_performance.data(), list_of_performance.size());
            Eigen::Map<Eigen::VectorXd> e_volatility(list_of_volatility.data(), list_of_volatility.size());
            Eigen::Map<Eigen::VectorXd> e_drawdown(list_of_drawdown.data(), list_of_drawdown.size());

            double mean_performance = e_performance.mean();
            double mean_volatility = e_volatility.mean();
            double mean_drawdown = e_drawdown.mean();

            double std_dev_performance = std::sqrt((e_performance.array() - mean_performance).square().mean());
            double std_dev_volatility = std::sqrt((e_volatility.array() - mean_volatility).square().mean());
            double std_dev_drawdown = std::sqrt((e_drawdown.array() - mean_drawdown).square().mean());

            double z_performance = (std_dev_performance != 0.0) ? (features[i][0] - mean_performance) / std_dev_performance : 0.0;
            double z_volatility = (std_dev_volatility != 0.0) ? (features[i][1] - mean_volatility) / std_dev_volatility : 0.0;
            double z_drawdown = (std_dev_drawdown != 0.0) ? (features[i][2] - mean_drawdown) / std_dev_drawdown : 0.0;

            features_normalized.push_back({ z_performance, z_volatility, z_drawdown });
        }
    }
}
