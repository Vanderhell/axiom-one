#include "one/rate.h"

one_rate_status_t one_rate_step(uint32_t numerator, uint32_t denominator, uint32_t *phase, uint8_t *emit)
{
    uint32_t next;
    if ((phase == 0) || (emit == 0)) { return ONE_RATE_ERR_INVALID_ARGUMENT; }
    if ((denominator == 0u) || (numerator > denominator)) { return ONE_RATE_ERR_INVALID_RATE; }
    if (*phase >= denominator) { return ONE_RATE_ERR_INVALID_PHASE; }
    if (numerator == 0u) { *emit = 0u; return ONE_RATE_OK; }
    if (*phase >= denominator - numerator) { next = *phase - (denominator - numerator); *emit = 1u; }
    else { next = *phase + numerator; *emit = 0u; }
    *phase = next;
    return ONE_RATE_OK;
}
