#ifndef ONE_EMA_H
#define ONE_EMA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ONE_EMA_MAX_SHIFT 31u

typedef struct { int64_t accumulator; uint8_t shift; } one_ema_t;
void one_ema_init(one_ema_t *ema, int32_t initial, uint8_t shift);
int32_t one_ema_update(one_ema_t *ema, int32_t sample);

#ifdef __cplusplus
}
#endif
#endif
