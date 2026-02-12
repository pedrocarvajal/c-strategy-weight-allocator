#include "helpers/headers/date_key_from_datetime.hpp"

namespace helpers {
    int date_key_from_datetime(const std::tm &datetime) {
        return (datetime.tm_year + 1900) * 10000
               + (datetime.tm_mon + 1) * 100
               + datetime.tm_mday;
    }
}
