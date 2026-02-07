#include <iostream>
#include "services/logger/consts/color.hpp"
#include "services/logger/enums/level.hpp"
#include "services/datetime/headers/datetime.hpp"

namespace services {
class Logger {
private:
    std::string prefix;

public:
    Logger(const std::string prefix = "") {
        this->prefix = prefix;
    }

    void info(const std::string message) {
        std::cout << getFormattedMessage(enums::Level::INFO, message);
    }

private:
    std::string getFormattedMessage(
        enums::Level severity,
        const std::string message
        ) {
        return getTime() + getLevel(severity) + getPrefix() + " > " + message;
    }

    std::string getTime() {
        DateTime now;
        return "[" + now.getFormatted() + "]";
    }

    std::string getLevel(enums::Level severity) {
        if (severity == enums::Level::INFO)
            return consts::color::WHITE + "[INFO]" + consts::color::RESET;

        return "";
    }

    std::string getPrefix() {
        if (!prefix.empty())
            return "[" + prefix + "]";

        return "";
    }
};
}
