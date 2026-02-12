#pragma once

#include "structs/strategy_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    std::map<int, std::vector<structs::StrategySnapshot> > get_strategy_snapshots(
        const nlohmann::json data,
        std::map<int, std::vector<structs::StrategySnapshot> > &snapshots
    );
}
