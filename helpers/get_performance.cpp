#include "structs/performance.hpp"

namespace helpers {
    structs::Performance get_performance(
        double current,
        double previous
    ) {
        structs::Performance performance;

        if (previous == 0)
            return performance;

        performance.amount = current - previous;
        performance.percentage = performance.amount / previous;
        return performance;
    }
}
