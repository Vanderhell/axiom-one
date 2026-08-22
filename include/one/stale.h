#ifndef ONE_STALE_H
#define ONE_STALE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { ONE_STALE_FRESH = 0, ONE_STALE_STALE, ONE_STALE_AMBIGUOUS } one_stale_result_t;
one_stale_result_t one_stale_check(uint32_t now, uint32_t updated_at, uint32_t max_age);

#ifdef __cplusplus
}
#endif
#endif
