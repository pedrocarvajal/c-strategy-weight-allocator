#include "helpers/headers/get_strategy_snapshots.hpp"

#include "structs/strategy_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    void get_strategy_snapshots(
        const nlohmann::json &data,
        std::vector<structs::StrategySnapshot> &snapshots
    ) {
        for (auto& snapshot: data) {
            long timestamp = snapshot["timestamp"];
            std::time_t time = static_cast<std::time_t>(timestamp);
            std::tm *datetime = std::localtime(&time);

            structs::StrategySnapshot snap;
            snap.datetime = *datetime;
            snap.strategy_symbol = snapshot.value("strategy_symbol", "");
            snap.strategy_name = snapshot.value("strategy_name", "");
            snap.strategy_prefix = snapshot.value("strategy_prefix", "");
            snap.nav = snapshot.value("nav", 0.0);
            snap.performance = snapshot.value("performance", 0.0);
            snap.daily_performance = snapshot.value("daily_performance", 0.0);

            snapshots.push_back(snap);
        }
    }
}
