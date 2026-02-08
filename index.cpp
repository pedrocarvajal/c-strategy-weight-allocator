#include "services/logger/headers/logger.hpp"

int main() {
    services::Logger logger("index");
    logger.info("Hello world...");
    logger.error("Hello world...");
    logger.success("Hello world...");
    logger.warning("Hello world...");
}
