#include <ctime>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>
#include <regex>

#include "structs/strategy_snapshot.hpp"
#include "structs/market_snapshot.hpp"

#include "services/logger/headers/logger.hpp"

#include "helpers/headers/get_market_snapshots.hpp"
#include "helpers/headers/get_strategy_snapshots.hpp"

#include "libraries/json.hpp"

int main() {
    namespace fs = std::filesystem;
    services::Logger logger("index");

    std::regex pattern(".*_Snapshots\\.json$");
    std::vector<nlohmann::json> files;
    std::map<int, std::vector<structs::StrategySnapshot> > strategy_snapshots;
    std::map<int, std::vector<structs::MarketSnapshot> > market_snapshots;

    for (auto& entry : fs::directory_iterator("storage/")) {
        std::string filename = entry.path().filename().string();

        if (std::regex_match(filename, pattern)) {
            std::ifstream file(entry.path());
            nlohmann::json data = nlohmann::json::parse(file);
            std::string level = data[0].value("level", "");

            if (level == "market") {
                market_snapshots = helpers::get_market_snapshots(
                    data,
                    market_snapshots
                );
            } else if (level == "strategy") {
                strategy_snapshots = helpers::get_strategy_snapshots(
                    data,
                    strategy_snapshots
                );
            } else {
                throw std::runtime_error(
                    "Unrecognized snapshot level '" + level
                    + "' in file: " + filename
                    + ". Expected 'market' or 'strategy'."
                );
            }
        }
    }

    if (market_snapshots.empty())
        throw std::runtime_error(
            "No market snapshots loaded."
            " Verify that 'storage/' contains '*_Snapshots.json' files with level 'market'."
        );

    if (strategy_snapshots.empty())
        throw std::runtime_error(
            "No strategy snapshots loaded."
            " Verify that 'storage/' contains '*_Snapshots.json' files with level 'strategy'."
        );
}
