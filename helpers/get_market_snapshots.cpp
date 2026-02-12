#include "helpers/headers/get_market_snapshots.hpp"
#include "helpers/headers/get_performance.hpp"

#include "structs/performance.hpp"
#include "structs/market_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    std::map<int, std::vector<structs::MarketSnapshot> > get_market_snapshots(
        const nlohmann::json data,
        std::map<int, std::vector<structs::MarketSnapshot> > &snapshots
    ) {
        std::string level = data[0].value("level", "");
        long prev_timestamp = 0;

        for (auto& snapshot: data) {
            long timestamp = snapshot["timestamp"];
            std::time_t time = static_cast<std::time_t>(timestamp);
            std::tm *datetime = std::localtime(&time);
            std::string symbol = snapshot.value("key", "");

            std::string market_symbol = snapshot.value("symbol", "");

            double bid = snapshot.value("bid", 0.0);
            double ask = snapshot.value("ask", 0.0);
            double daily_performance = 0.0;

            if (!snapshots.empty()) {
                auto iterator = snapshots.find(prev_timestamp);

                if (iterator != snapshots.end()) {
                    for (auto& prev_snap: iterator->second) {
                        if (prev_snap.symbol == symbol) {
                            structs::Performance p = get_performance(ask, prev_snap.ask);
                            daily_performance = p.percentage;
                        }
                    }
                }
            }

            structs::MarketSnapshot snap;
            snap.datetime = *datetime;
            snap.symbol = market_symbol;
            snap.bid = bid;
            snap.ask = ask;
            snap.daily_performance = daily_performance;

            snapshots[timestamp].push_back(snap);
        }

        return snapshots;
    }
}
