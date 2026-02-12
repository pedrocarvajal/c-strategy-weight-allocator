#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "structs/strategy_snapshot.hpp"

namespace helpers {
    void build_strategy_lookup(
        const std::vector<structs::StrategySnapshot> &strategy_snapshots,
        std::map<int, std::map<std::string, double>> &strategy_by_date,
        std::set<std::string> &strategy_names
    );
}
