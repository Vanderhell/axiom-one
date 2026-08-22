#include "one/hyst.h"

static one_hyst_status_t validate(uint8_t *state)
{
    if (state == 0) { return ONE_HYST_ERR_INVALID_ARGUMENT; }
    return (*state <= 1u) ? ONE_HYST_OK : ONE_HYST_ERR_INVALID_STATE;
}

one_hyst_status_t one_hyst_u32(uint32_t value, uint32_t low, uint32_t high, uint8_t *state)
{
    one_hyst_status_t status = validate(state);
    if (status != ONE_HYST_OK) { return status; }
    if (low >= high) { return ONE_HYST_ERR_INVALID_RANGE; }
    if (value <= low) { *state = 0u; } else if (value >= high) { *state = 1u; }
    return ONE_HYST_OK;
}

one_hyst_status_t one_hyst_i32(int32_t value, int32_t low, int32_t high, uint8_t *state)
{
    one_hyst_status_t status = validate(state);
    if (status != ONE_HYST_OK) { return status; }
    if (low >= high) { return ONE_HYST_ERR_INVALID_RANGE; }
    if (value <= low) { *state = 0u; } else if (value >= high) { *state = 1u; }
    return ONE_HYST_OK;
}
