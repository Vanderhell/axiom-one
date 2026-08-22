#ifndef ONE_SLEW_H
#define ONE_SLEW_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { ONE_SLEW_OK = 0, ONE_SLEW_ERR_INVALID_ARGUMENT } one_slew_status_t;
one_slew_status_t one_slew_u32(uint32_t target, uint32_t rise, uint32_t fall, uint32_t *current);
one_slew_status_t one_slew_i32(int32_t target, uint32_t rise, uint32_t fall, int32_t *current);
#ifdef __cplusplus
}
#endif
#endif
