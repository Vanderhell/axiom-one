#include "test_assert.h"
#include <stdint.h>

#include "one/arb.h"
#include "one/backoff.h"

static void test_arb(void)
{
    one_arb_t arb;
    uint8_t winner = 99u;
    uint8_t first;

    one_arb_init(&arb, 2u);
    assert(one_arb_next(&arb, (UINT32_C(1) << 2) | (UINT32_C(1) << 4), &winner));
    assert(winner == 2u);
    assert(arb.cursor == 3u);
    assert(one_arb_next(&arb, (UINT32_C(1) << 2) | (UINT32_C(1) << 4), &winner));
    assert(winner == 4u);
    assert(!one_arb_next(&arb, 0u, &winner));
    assert(winner == 4u);
    assert(arb.cursor == 5u);
    one_arb_init(&arb, 31u);
    assert(one_arb_next(&arb, UINT32_C(1), &winner));
    assert(winner == 0u && arb.cursor == 1u);
    assert(!one_arb_next(0, UINT32_C(1), &winner));
    assert(!one_arb_next(&arb, UINT32_C(1), 0));

    for (first = 0u; first < 32u; ++first) {
        uint8_t requested;
        one_arb_init(&arb, first);
        for (requested = 0u; requested < 32u; ++requested) {
            assert(one_arb_next(&arb, UINT32_C(1) << requested, &winner));
            assert(winner == requested && arb.cursor == (uint8_t)((requested + 1u) & 31u));
            one_arb_init(&arb, first);
        }
    }
    for (first = 0u; first < 32u; ++first) {
        uint32_t requests;
        for (requests = 1u; requests < UINT32_C(65536); ++requests) {
            uint8_t expected = first;
            while ((requests & (UINT32_C(1) << expected)) == 0u) expected = (uint8_t)((expected + 1u) & 31u);
            one_arb_init(&arb, first);
            assert(one_arb_next(&arb, requests, &winner));
            assert(winner == expected && arb.cursor == (uint8_t)((expected + 1u) & 31u));
        }
    }
    arb.cursor = 32u;
    assert(!one_arb_next(&arb, 1u, &winner));
}

static void test_backoff(void)
{
    uint32_t base;
    assert(one_backoff_delay(10u, 0u, 1000u) == 10u);
    assert(one_backoff_delay(10u, 6u, 1000u) == 640u);
    assert(one_backoff_delay(10u, 7u, 1000u) == 1000u);
    assert(one_backoff_delay(125u, 3u, 1000u) == 1000u);
    assert(one_backoff_delay(1u, UINT32_MAX, 1000u) == 1000u);
    assert(one_backoff_delay(0u, 4u, 1000u) == 0u);
    assert(one_backoff_delay(5u, 4u, 0u) == 0u);
    assert(one_backoff_delay(UINT32_MAX, 1u, UINT32_MAX) == UINT32_MAX);
    assert(one_backoff_delay(1u, 31u, UINT32_MAX) == UINT32_C(0x80000000));

    for (base = 0u; base <= 128u; ++base) {
        uint32_t maximum;
        for (maximum = 0u; maximum <= 256u; ++maximum) {
            uint32_t attempt;
            for (attempt = 0u; attempt <= 40u; ++attempt) {
                uint64_t reference = base;
                uint32_t step;
                for (step = 0u; step < attempt && reference < maximum; ++step) reference *= 2u;
                if (reference > maximum) reference = maximum;
                assert(one_backoff_delay(base, attempt, maximum) == (uint32_t)reference);
            }
        }
    }
}

int main(void)
{
    test_arb();
    test_backoff();
    return 0;
}
