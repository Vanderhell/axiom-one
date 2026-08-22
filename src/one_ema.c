#include "one/ema.h"

void one_ema_init(one_ema_t *ema, int32_t initial, uint8_t shift)
{
    if ((ema != 0) && (shift <= ONE_EMA_MAX_SHIFT)) {
        ema->accumulator = (int64_t)initial * ((int64_t)1 << shift);
        ema->shift = shift;
    }
}

int32_t one_ema_update(one_ema_t *ema, int32_t sample)
{
    int64_t scale;
    int64_t current;
    if ((ema == 0) || (ema->shift > ONE_EMA_MAX_SHIFT)) { return 0; }
    scale = (int64_t)1 << ema->shift;
    current = ema->accumulator / scale;
    ema->accumulator += (int64_t)sample - current;
    return (int32_t)(ema->accumulator / scale);
}
