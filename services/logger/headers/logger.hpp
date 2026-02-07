#pragma once

#include <string>
#include "services/logger/enums/level.hpp"

namespace services {
class Logger {
public:
    Logger(const std::string prefix = "");
    void info(const std::string message);

private:
    std::string getFormattedMessage();
    std::string getTime();
    std::string getLevel(enums::Level severity);
    std::string getPrefix();
};
}
