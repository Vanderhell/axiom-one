#ifndef ONE_BACKOFF_H
#define ONE_BACKOFF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t one_backoff_delay(uint32_t base, uint32_t attempt, uint32_t maximum);

#ifdef __cplusplus
}
#endif

#endif
