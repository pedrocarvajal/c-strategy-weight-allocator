#pragma once

#include "structs/market_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    void get_market_snapshots(
        const nlohmann::json &data,
        std::vector<structs::MarketSnapshot> &snapshots,
        std::vector<int> &timestamps
    );
}
