#include "one/stale.h"

one_stale_result_t one_stale_check(uint32_t now, uint32_t updated_at, uint32_t max_age)
{
    const uint32_t age = now - updated_at;
    if ((max_age >= UINT32_C(0x80000000)) || (age >= UINT32_C(0x80000000))) {
        return ONE_STALE_AMBIGUOUS;
    }
    return (age <= max_age) ? ONE_STALE_FRESH : ONE_STALE_STALE;
}
