#pragma once

#include <ctime>
#include <string>
#include <variant>

#include "structs/market_snapshot.hpp"
#include "structs/strategy_snapshot.hpp"

namespace structs {
    struct Snapshot {
        std::tm datetime;
        std::string symbol;
        std::string type;
        std::variant<structs::MarketSnapshot, structs::StrategySnapshot> snapshot;
    };
}
