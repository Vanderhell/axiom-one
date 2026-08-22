#ifndef ONE_SCALE_H
#define ONE_SCALE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_scale_status {
    ONESCALE_OK = 0,
    ONESCALE_ERR_INVALID_ARGUMENT,
    ONESCALE_ERR_INVALID_RANGE,
    ONESCALE_ERR_OUT_OF_RANGE
} one_scale_status_t;

one_scale_status_t one_scale_u32(uint32_t value, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max, uint32_t *out);
one_scale_status_t one_scale_i32(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, int32_t *out);
one_scale_status_t one_scale_u32_clamp(uint32_t value, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max, uint32_t *out);
one_scale_status_t one_scale_i32_clamp(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, int32_t *out);

#ifdef __cplusplus
}
#endif

#endif
