#pragma once

#include "structs/strategy_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    void get_strategy_snapshots(
        const nlohmann::json &data,
        std::vector<structs::StrategySnapshot> &snapshots
    );
}
