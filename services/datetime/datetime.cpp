#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace services {
class DateTime {
public:
    std::tm datetime;

    DateTime(const std::string& seed = "") {
        if (!seed.empty()) {
            std::tm tm = {};
            std::istringstream stream(seed);
            stream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            datetime = tm;
        }

        std::time_t now = std::time(nullptr);
        datetime = *std::localtime(&now);
    }

    void addDays(int days) {
        datetime.tm_mday += days;
        refresh();
    }

    void addMinutes(int minutes) {
        datetime.tm_min += minutes;
        refresh();
    }

    void addSeconds(int seconds) {
        datetime.tm_sec += seconds;
        refresh();
    }

    float getTimestamp() {
        return std::mktime(&datetime);
    }

    std::string getFormatted(const std::string format = "%Y-%m-%d %H:%M:%S") {
        std::ostringstream stream;
        stream << std::put_time(&datetime, format.c_str());
        return stream.str();
    }

    int getDay() {
        return datetime.tm_mday;
    }

    int getHour() {
        return datetime.tm_hour;
    }

    int getMinute() {
        return datetime.tm_min;
    }

    int getSecond() {
        return datetime.tm_sec;
    }

private:
    void refresh() {
        std::mktime(&datetime);
    }
};
}
