#pragma once

#include <string>

namespace services {
class DateTime {
public:
    std::tm datetime;

public:
    DateTime(const std::string &seed = "");
    void addDays(int days);
    void addMinutes(int minutes);
    void addSeconds(int seconds);
    float getTimestamp();
    std::string getFormatted(const std::string format = "%Y-%m-%d %H:%M:%S");
    int getDay();
    int getHour();
    int getMinute();
    int getSecond();

private:
    void refresh();
};
}
