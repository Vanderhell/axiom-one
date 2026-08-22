#include "one/outlier.h"

void one_outlier_init(one_outlier_t *state, int32_t initial, uint32_t max_delta)
{
    if (state != 0) { state->accepted = initial; state->max_delta = max_delta; }
}

bool one_outlier_accept(one_outlier_t *state, int32_t sample)
{
    uint32_t difference;
    if (state == 0) { return false; }
    difference = (sample >= state->accepted)
        ? (uint32_t)((int64_t)sample - (int64_t)state->accepted)
        : (uint32_t)((int64_t)state->accepted - (int64_t)sample);
    if (difference > state->max_delta) { return false; }
    state->accepted = sample;
    return true;
}
