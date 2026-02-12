#include "helpers/headers/get_strategy_snapshots.hpp"
#include "helpers/headers/get_performance.hpp"

#include "structs/performance.hpp"
#include "structs/strategy_snapshot.hpp"

#include "libraries/json.hpp"

namespace helpers {
    std::map<int, std::vector<structs::StrategySnapshot> > get_strategy_snapshots(
        const nlohmann::json data,
        std::map<int, std::vector<structs::StrategySnapshot> > &snapshots
    ) {
        std::string level = data[0].value("level", "");
        long prev_timestamp = 0;

        for (auto& snapshot: data) {
            long timestamp = snapshot["timestamp"];
            std::time_t time = static_cast<std::time_t>(timestamp);
            std::tm *datetime = std::localtime(&time);
            std::string symbol = snapshot.value("key", "");

            std::string strategy_symbol = snapshot.value("strategy_symbol", "");
            std::string strategy_name = snapshot.value("strategy_name", "");
            std::string strategy_prefix = snapshot.value("strategy_prefix", "");

            double nav = snapshot.value("nav", 0.0);
            double performance = snapshot.value("performance", 0.0);
            double daily_performance = 0.0;

            if (!snapshots.empty()) {
                auto iterator = snapshots.find(prev_timestamp);

                if (iterator != snapshots.end()) {
                    for (auto& prev_snap: iterator->second) {
                        if (prev_snap.strategy_symbol == symbol && prev_snap.strategy_prefix == strategy_prefix) {
                            structs::Performance p = get_performance(performance, prev_snap.performance);
                            daily_performance = p.percentage;
                        }
                    }
                }
            }

            structs::StrategySnapshot snap;
            snap.datetime = *datetime;
            snap.strategy_symbol = strategy_symbol;
            snap.strategy_name = strategy_name;
            snap.strategy_prefix = strategy_prefix;
            snap.nav = nav;
            snap.performance = performance;
            snap.daily_performance = daily_performance;

            snapshots[timestamp].push_back(snap);
        }

        return snapshots;
    }
}
