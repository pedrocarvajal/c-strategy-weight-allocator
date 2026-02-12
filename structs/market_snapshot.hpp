#pragma once

#include <ctime>
#include <string>

namespace structs {
    struct MarketSnapshot {
        std::tm datetime;
        std::string symbol;
        double bid;
        double ask;
        double performance;
        double momentum;
        double drawdown;
        double volatility;
        double daily_performance;
    };
}
