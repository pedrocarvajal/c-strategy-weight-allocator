#pragma once

#include "structs/performance.hpp"

namespace helpers {
    structs::Performance get_performance(
        double current,
        double previous
    );
}
