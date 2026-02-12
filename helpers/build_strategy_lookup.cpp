#include "helpers/headers/build_strategy_lookup.hpp"

#include "helpers/headers/date_key_from_datetime.hpp"

namespace helpers {
    void build_strategy_lookup(
        const std::vector<structs::StrategySnapshot> &strategy_snapshots,
        std::map<int, std::map<std::string, double>> &strategy_by_date,
        std::set<std::string> &strategy_names
    ) {
        for (const auto& snap : strategy_snapshots) {
            int date_key = helpers::date_key_from_datetime(snap.datetime);

            strategy_by_date[date_key][snap.strategy_name] = snap.daily_performance;
            strategy_names.insert(snap.strategy_name);
        }
    }
}
