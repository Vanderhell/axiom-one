#ifndef ONE_HYST_H
#define ONE_HYST_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { ONE_HYST_OK = 0, ONE_HYST_ERR_INVALID_ARGUMENT, ONE_HYST_ERR_INVALID_RANGE, ONE_HYST_ERR_INVALID_STATE } one_hyst_status_t;
one_hyst_status_t one_hyst_u32(uint32_t value, uint32_t low, uint32_t high, uint8_t *state);
one_hyst_status_t one_hyst_i32(int32_t value, int32_t low, int32_t high, uint8_t *state);
#ifdef __cplusplus
}
#endif
#endif
