#pragma once

#include <ctime>
#include <string>

namespace structs {
    struct MarketSnapshot {
        std::tm datetime;
        std::string symbol;
        double bid;
        double ask;
        double performance; // rolling performance
        double drawdown; // rolling drawdown
        double volatility; // rolling volatility
        double daily_performance;
    };
}
