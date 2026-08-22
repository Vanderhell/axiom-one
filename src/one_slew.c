#include "one/slew.h"

one_slew_status_t one_slew_u32(uint32_t target, uint32_t rise, uint32_t fall, uint32_t *current)
{
    uint32_t distance;
    if (current == 0) { return ONE_SLEW_ERR_INVALID_ARGUMENT; }
    if (target > *current) { distance = target - *current; *current = (distance <= rise) ? target : *current + rise; }
    else if (target < *current) { distance = *current - target; *current = (distance <= fall) ? target : *current - fall; }
    return ONE_SLEW_OK;
}

one_slew_status_t one_slew_i32(int32_t target, uint32_t rise, uint32_t fall, int32_t *current)
{
    int64_t delta;
    if (current == 0) { return ONE_SLEW_ERR_INVALID_ARGUMENT; }
    delta = (int64_t)target - (int64_t)*current;
    if (delta > 0) { *current = ((uint64_t)delta <= rise) ? target : (int32_t)((int64_t)*current + rise); }
    else if (delta < 0) { const uint64_t distance = (uint64_t)(-delta); *current = (distance <= fall) ? target : (int32_t)((int64_t)*current - fall); }
    return ONE_SLEW_OK;
}
