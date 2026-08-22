#ifndef ONE_RATE_H
#define ONE_RATE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { ONE_RATE_OK = 0, ONE_RATE_ERR_INVALID_ARGUMENT, ONE_RATE_ERR_INVALID_RATE, ONE_RATE_ERR_INVALID_PHASE } one_rate_status_t;
one_rate_status_t one_rate_step(uint32_t numerator, uint32_t denominator, uint32_t *phase, uint8_t *emit);
#ifdef __cplusplus
}
#endif
#endif
