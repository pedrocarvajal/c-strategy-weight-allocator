#pragma once

#include <ctime>
#include <string>

namespace structs {
    struct MarketSnapshot {
        std::tm datetime;
        std::string symbol;
        double bid;
        double ask;
        double daily_performance;
    };
}
