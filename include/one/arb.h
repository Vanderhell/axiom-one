#ifndef ONE_ARB_H
#define ONE_ARB_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t cursor;
} one_arb_t;

void one_arb_init(one_arb_t *arb, uint8_t first);
bool one_arb_next(one_arb_t *arb, uint32_t requests, uint8_t *winner);

#ifdef __cplusplus
}
#endif

#endif
