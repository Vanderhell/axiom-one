#ifndef ONE_OUTLIER_H
#define ONE_OUTLIER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { int32_t accepted; uint32_t max_delta; } one_outlier_t;
void one_outlier_init(one_outlier_t *state, int32_t initial, uint32_t max_delta);
bool one_outlier_accept(one_outlier_t *state, int32_t sample);

#ifdef __cplusplus
}
#endif
#endif
