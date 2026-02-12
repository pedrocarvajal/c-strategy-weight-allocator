#include "helpers/headers/get_market_snapshots.hpp"

#include "structs/market_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    std::map<int, std::vector<structs::MarketSnapshot> > get_market_snapshots(
        const nlohmann::json data,
        std::map<int, std::vector<structs::MarketSnapshot> > &snapshots
    ) {
        for (auto& snapshot: data) {
            long timestamp = snapshot["timestamp"];
            std::time_t time = static_cast<std::time_t>(timestamp);
            std::tm *datetime = std::localtime(&time);

            structs::MarketSnapshot snap;
            snap.datetime = *datetime;
            snap.symbol = snapshot.value("symbol", "");
            snap.bid = snapshot.value("bid", 0.0);
            snap.ask = snapshot.value("ask", 0.0);
            snap.performance = snapshot.value("performance", 0.0);
            snap.momentum = snapshot.value("momentum", 0.0);
            snap.drawdown = snapshot.value("drawdown", 0.0);
            snap.volatility = snapshot.value("volatility", 0.0);
            snap.daily_performance = snapshot.value("daily_performance", 0.0);

            snapshots[timestamp].push_back(snap);
        }

        return snapshots;
    }
}
