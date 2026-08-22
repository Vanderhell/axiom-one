#include "one/quant.h"

static int64_t one_quant_nearest(int64_t value, int64_t origin, uint64_t step,
                                 int64_t minimum, int64_t maximum)
{
    uint64_t distance = value >= origin ? (uint64_t)(value - origin) : (uint64_t)(origin - value);
    uint64_t quotient = distance / step;
    uint64_t remainder = distance % step;
    uint64_t offset = quotient * step;
    int64_t toward = value >= origin ? origin + (int64_t)offset : origin - (int64_t)offset;
    int outward_exists;

    if (remainder == 0u) return toward;
    outward_exists = value >= origin ? (maximum - toward >= (int64_t)step)
                                     : (toward - minimum >= (int64_t)step);
    if (outward_exists && remainder >= step - remainder)
        return value >= origin ? toward + (int64_t)step : toward - (int64_t)step;
    return toward;
}

one_quant_status_t one_quant_u32(uint32_t value, uint32_t origin, uint32_t step, uint32_t *out)
{
    if (out == 0) return ONEQUANT_ERR_INVALID_ARGUMENT;
    if (step == 0u) return ONEQUANT_ERR_INVALID_STEP;
    *out = (uint32_t)one_quant_nearest((int64_t)value, (int64_t)origin, step, 0, UINT32_MAX);
    return ONEQUANT_OK;
}

one_quant_status_t one_quant_i32(int32_t value, int32_t origin, uint32_t step, int32_t *out)
{
    if (out == 0) return ONEQUANT_ERR_INVALID_ARGUMENT;
    if (step == 0u) return ONEQUANT_ERR_INVALID_STEP;
    *out = (int32_t)one_quant_nearest(value, origin, step, INT32_MIN, INT32_MAX);
    return ONEQUANT_OK;
}
