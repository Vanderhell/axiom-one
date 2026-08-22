#include "one/dead.h"

one_dead_status_t one_dead_u32(uint32_t value, uint32_t low, uint32_t high, uint32_t *out)
{
    if (out == 0) { return ONE_DEAD_ERR_INVALID_ARGUMENT; }
    if (low > high) { return ONE_DEAD_ERR_INVALID_RANGE; }
    *out = ((value >= low) && (value <= high)) ? 0u : value;
    return ONE_DEAD_OK;
}

one_dead_status_t one_dead_i32(int32_t value, int32_t low, int32_t high, int32_t *out)
{
    if (out == 0) { return ONE_DEAD_ERR_INVALID_ARGUMENT; }
    if (low > high) { return ONE_DEAD_ERR_INVALID_RANGE; }
    *out = ((value >= low) && (value <= high)) ? 0 : value;
    return ONE_DEAD_OK;
}
