#include "services/logger/headers/logger.hpp"
#include "libraries/csvmonkey.hpp"
#include "libraries/json.hpp"

int main() {
    services::Logger logger("index");
    logger.info("Starting...");
    logger.error("Error");
}
