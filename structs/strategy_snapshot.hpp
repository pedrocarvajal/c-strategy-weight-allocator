#pragma once

#include <ctime>
#include <string>

namespace structs {
    struct StrategySnapshot {
        std::tm datetime;
        std::string strategy_symbol;
        std::string strategy_name;
        std::string strategy_prefix;
        double nav;
        double performance;
        double daily_performance;
    };
}
