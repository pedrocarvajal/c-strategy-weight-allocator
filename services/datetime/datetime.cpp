#include <ctime>
#include <iomanip>
#include <sstream>
#include "services/datetime/headers/datetime.hpp"

namespace services {
    DateTime::DateTime(const std::string &seed) {
        if (!seed.empty()) {
            std::tm tm = {};
            std::istringstream stream(seed);
            stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            datetime = tm;
        }

        std::time_t now = std::time(nullptr);
        datetime = *std::localtime(&now);
    }

    void DateTime::addDays(int days) {
        datetime.tm_mday += days;
        refresh();
    }

    void DateTime::addMinutes(int minutes) {
        datetime.tm_min += minutes;
        refresh();
    }

    void DateTime::addSeconds(int seconds) {
        datetime.tm_sec += seconds;
        refresh();
    }

    float DateTime::getTimestamp() {
        return std::mktime(&datetime);
    }

    std::string DateTime::getFormatted(const std::string format) {
        std::ostringstream stream;
        stream << std::put_time(&datetime, format.c_str());
        return stream.str();
    }

    int DateTime::getDay() {
        return datetime.tm_mday;
    }

    int DateTime::getHour() {
        return datetime.tm_hour;
    }

    int DateTime::getMinute() {
        return datetime.tm_min;
    }

    int DateTime::getSecond() {
        return datetime.tm_sec;
    }

    void DateTime::refresh() {
        std::mktime(&datetime);
    }
}
