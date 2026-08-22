#include "one/window.h"

void one_window_reset(one_window_t *window)
{
    if (window != 0) { window->newest = 0u; window->seen = 0u; }
}

one_window_result_t one_window_accept(one_window_t *window, uint32_t sequence)
{
    uint32_t distance;
    if (window == 0) { return ONE_WINDOW_AMBIGUOUS; }
    if (window->seen == 0u) { window->newest = sequence; window->seen = 1u; return ONE_WINDOW_NEW; }
    distance = sequence - window->newest;
    if (distance == 0u) { return ONE_WINDOW_DUPLICATE; }
    if (distance == UINT32_C(0x80000000)) { return ONE_WINDOW_AMBIGUOUS; }
    if (distance < UINT32_C(0x80000000)) {
        window->seen = (distance >= 32u) ? 1u : ((window->seen << distance) | 1u);
        window->newest = sequence;
        return ONE_WINDOW_NEW;
    }
    distance = window->newest - sequence;
    if (distance >= 32u) { return ONE_WINDOW_TOO_OLD; }
    if ((window->seen & (UINT32_C(1) << distance)) != 0u) { return ONE_WINDOW_DUPLICATE; }
    window->seen |= UINT32_C(1) << distance;
    return ONE_WINDOW_NEW;
}
