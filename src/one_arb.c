#include "one/arb.h"

void one_arb_init(one_arb_t *arb, uint8_t first)
{
    if (arb != 0) {
        arb->cursor = (uint8_t)(first & 31u);
    }
}

bool one_arb_next(one_arb_t *arb, uint32_t requests, uint8_t *winner)
{
    uint8_t offset;

    if ((arb == 0) || (winner == 0) || (arb->cursor > 31u) || (requests == 0u)) {
        return false;
    }

    for (offset = 0u; offset < 32u; ++offset) {
        const uint8_t candidate = (uint8_t)((arb->cursor + offset) & 31u);

        if ((requests & ((uint32_t)1u << candidate)) != 0u) {
            *winner = candidate;
            arb->cursor = (uint8_t)((candidate + 1u) & 31u);
            return true;
        }
    }

    return false;
}
