#pragma once

#include "structs/market_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    std::map<int, std::vector<structs::MarketSnapshot> > get_market_snapshots(
        const nlohmann::json data,
        std::map<int, std::vector<structs::MarketSnapshot> > &snapshots
    );
}
