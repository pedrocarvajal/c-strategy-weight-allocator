#pragma once

#include <vector>

#include "structs/market_snapshot.hpp"

namespace helpers {
    void normalize_features(
        const std::vector<structs::MarketSnapshot> &market_snapshots,
        const std::vector<int> &timestamps,
        int normalization_window,
        std::vector<std::vector<double>> &features,
        std::vector<std::vector<double>> &features_normalized
    );
}
