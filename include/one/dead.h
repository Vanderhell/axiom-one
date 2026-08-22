#ifndef ONE_DEAD_H
#define ONE_DEAD_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { ONE_DEAD_OK = 0, ONE_DEAD_ERR_INVALID_ARGUMENT, ONE_DEAD_ERR_INVALID_RANGE } one_dead_status_t;
one_dead_status_t one_dead_u32(uint32_t value, uint32_t low, uint32_t high, uint32_t *out);
one_dead_status_t one_dead_i32(int32_t value, int32_t low, int32_t high, int32_t *out);
#ifdef __cplusplus
}
#endif
#endif
