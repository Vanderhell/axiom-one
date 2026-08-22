#ifndef ONE_QUANT_H
#define ONE_QUANT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_quant_status {
    ONEQUANT_OK = 0,
    ONEQUANT_ERR_INVALID_ARGUMENT,
    ONEQUANT_ERR_INVALID_STEP
} one_quant_status_t;

one_quant_status_t one_quant_u32(uint32_t value, uint32_t origin, uint32_t step, uint32_t *out);
one_quant_status_t one_quant_i32(int32_t value, int32_t origin, uint32_t step, int32_t *out);

#ifdef __cplusplus
}
#endif

#endif
