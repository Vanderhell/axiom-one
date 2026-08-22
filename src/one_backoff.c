#include "one/backoff.h"

uint32_t one_backoff_delay(uint32_t base, uint32_t attempt, uint32_t maximum)
{
    uint32_t value;
    uint32_t step;

    if ((maximum == 0u) || (base == 0u)) {
        return 0u;
    }
    if (base >= maximum) {
        return maximum;
    }

    value = base;
    for (step = 0u; (step < 32u) && (step < attempt); ++step) {
        if (value > (maximum / 2u)) {
            return maximum;
        }
        value *= 2u;
        if (value >= maximum) {
            return maximum;
        }
    }
    return value;
}
