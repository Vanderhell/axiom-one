#include "test_assert.h"
#include <limits.h>
#include <stddef.h>
#include "one/ema.h"
#include "one/outlier.h"
#include "one/stale.h"
#include "one/vote.h"

static bool outlier_reference_accept(int32_t accepted, uint32_t max_delta, int32_t sample)
{
    uint64_t difference = sample >= accepted
        ? (uint64_t)((int64_t)sample - accepted)
        : (uint64_t)((int64_t)accepted - sample);
    return difference <= max_delta;
}

static void test_outlier_exhaustive(void)
{
    int32_t accepted;

    for (accepted = -32; accepted <= 32; ++accepted) {
        int32_t sample;
        for (sample = -32; sample <= 32; ++sample) {
            uint32_t max_delta;
            for (max_delta = 0u; max_delta <= 64u; ++max_delta) {
                one_outlier_t state;
                bool expected = outlier_reference_accept(accepted, max_delta, sample);
                one_outlier_init(&state, accepted, max_delta);
                assert(one_outlier_accept(&state, sample) == expected);
                assert(state.accepted == (expected ? sample : accepted));
                assert(state.max_delta == max_delta);
            }
        }
    }
    {
        static const int32_t values[] = {
            INT32_MIN, INT32_MIN + 1, -1, 0, 1, INT32_MAX - 1, INT32_MAX
        };
        static const uint32_t limits[] = { 0u, 1u, 2u, UINT32_MAX - 1u, UINT32_MAX };
        size_t accepted_index;

        for (accepted_index = 0u; accepted_index < sizeof(values) / sizeof(values[0]); ++accepted_index) {
            size_t sample_index;
            for (sample_index = 0u; sample_index < sizeof(values) / sizeof(values[0]); ++sample_index) {
                size_t limit_index;
                for (limit_index = 0u; limit_index < sizeof(limits) / sizeof(limits[0]); ++limit_index) {
                    one_outlier_t state;
                    int32_t before = values[accepted_index];
                    bool expected = outlier_reference_accept(before, limits[limit_index], values[sample_index]);
                    one_outlier_init(&state, before, limits[limit_index]);
                    assert(one_outlier_accept(&state, values[sample_index]) == expected);
                    assert(state.accepted == (expected ? values[sample_index] : before));
                    assert(state.max_delta == limits[limit_index]);
                }
            }
        }
    }
    assert(!one_outlier_accept(0, 0));
    one_outlier_init(0, 123, 456u);
}

static bool vote_reference_push(uint32_t *bits, uint8_t *count, uint8_t window, uint8_t required, bool value)
{
    uint32_t oldest = (*bits >> (window - 1u)) & 1u;
    uint32_t mask = window == ONE_VOTE_MAX_WINDOW ? UINT32_MAX : ((UINT32_C(1) << window) - 1u);

    if (oldest != 0u) --*count;
    *bits = ((*bits << 1u) | (value ? 1u : 0u)) & mask;
    if (value) ++*count;
    return *count >= required;
}

static void test_vote_exhaustive(void)
{
    uint8_t window;

    for (window = 1u; window <= ONE_VOTE_MAX_WINDOW; ++window) {
        uint8_t required;
        for (required = 1u; required <= window; ++required) {
            uint32_t sequence;
            for (sequence = 0u; sequence < UINT32_C(1) << 8u; ++sequence) {
                one_vote_t vote;
                uint32_t bits = 0u;
                uint8_t count = 0u;
                uint8_t step;

                assert(one_vote_init(&vote, window, required));
                for (step = 0u; step < 8u; ++step) {
                    bool value = ((sequence >> step) & 1u) != 0u;
                    bool expected = vote_reference_push(&bits, &count, window, required, value);

                    assert(one_vote_push(&vote, value) == expected);
                    assert(one_vote_get(&vote) == expected);
                    assert(vote.bits == bits && vote.count == count);
                    assert(vote.window == window && vote.required == required);
                }
                one_vote_reset(&vote);
                assert(vote.bits == 0u && vote.count == 0u);
                assert(!one_vote_get(&vote));
            }
            {
                one_vote_t vote;
                uint32_t bits = 0u;
                uint8_t count = 0u;
                uint32_t state = UINT32_C(0x9e3779b9);
                uint8_t step;
                assert(one_vote_init(&vote, window, required));
                for (step = 0u; step < 64u; ++step) {
                    bool value;
                    state = state * UINT32_C(1664525) + UINT32_C(1013904223);
                    value = (state >> 31u) != 0u;
                    assert(one_vote_push(&vote, value) == vote_reference_push(&bits, &count, window, required, value));
                    assert(vote.bits == bits && vote.count == count);
                }
            }
        }
    }
    {
        one_vote_t vote = { UINT32_MAX, 7u, 5u, 4u };
        one_vote_t original = vote;

        assert(!one_vote_init(&vote, 0u, 1u) && vote.bits == original.bits && vote.window == original.window && vote.required == original.required && vote.count == original.count);
        assert(!one_vote_init(&vote, ONE_VOTE_MAX_WINDOW + 1u, 1u));
        assert(!one_vote_init(&vote, 1u, 0u));
        assert(!one_vote_init(&vote, 1u, 2u));
        assert(!one_vote_init(0, 1u, 1u));
        assert(!one_vote_push(0, false));
        assert(!one_vote_get(0));
        one_vote_reset(0);
    }
}

static void test_stale_wrap_boundaries(void)
{
    static const uint32_t values[] = {
        0u, 1u, UINT32_C(0x7ffffffe), UINT32_C(0x7fffffff),
        UINT32_C(0x80000000), UINT32_C(0x80000001), UINT32_MAX
    };
    size_t now_index;

    for (now_index = 0u; now_index < sizeof(values) / sizeof(values[0]); ++now_index) {
        size_t updated_index;
        for (updated_index = 0u; updated_index < sizeof(values) / sizeof(values[0]); ++updated_index) {
            size_t age_index;
            uint32_t age = values[now_index] - values[updated_index];
            for (age_index = 0u; age_index < sizeof(values) / sizeof(values[0]); ++age_index) {
                uint32_t max_age = values[age_index];
                one_stale_result_t expected = (max_age >= UINT32_C(0x80000000) || age >= UINT32_C(0x80000000))
                    ? ONE_STALE_AMBIGUOUS
                    : (age <= max_age ? ONE_STALE_FRESH : ONE_STALE_STALE);
                assert(one_stale_check(values[now_index], values[updated_index], max_age) == expected);
            }
        }
    }
}

static void test_ema_all_shifts(void)
{
    static const int32_t samples[] = {
        INT32_MIN, INT32_MIN + 1, -1, 0, 1, INT32_MAX - 1, INT32_MAX
    };
    uint8_t shift;

    for (shift = 0u; shift <= ONE_EMA_MAX_SHIFT; ++shift) {
        one_ema_t first;
        one_ema_t second;
        int32_t previous = 0;
        size_t i;

        one_ema_init(&first, 0, shift);
        one_ema_init(&second, 0, shift);
        for (i = 0u; i < sizeof(samples) / sizeof(samples[0]); ++i) {
            int32_t a = one_ema_update(&first, samples[i]);
            int32_t b = one_ema_update(&second, samples[i]);
            int32_t low = previous < samples[i] ? previous : samples[i];
            int32_t high = previous > samples[i] ? previous : samples[i];

            assert(a == b && first.accumulator == second.accumulator);
            assert(a >= low && a <= high);
            if (shift == 0u) assert(a == samples[i]);
            previous = a;
        }
    }
}

int main(void)
{
    one_outlier_t outlier;
    one_ema_t ema;
    one_vote_t vote;
    one_outlier_init(&outlier, 100, 5u);
    assert(one_outlier_accept(&outlier, 105));
    assert(!one_outlier_accept(&outlier, 111));
    assert(outlier.accepted == 105);
    assert(one_stale_check(16u, UINT_MAX - 15u, 32u) == ONE_STALE_FRESH);
    assert(one_stale_check(0u, UINT32_C(0x80000000), 1u) == ONE_STALE_AMBIGUOUS);
    one_ema_init(&ema, 99, 3u);
    for (unsigned i = 0; i < 8; ++i) { (void)one_ema_update(&ema, 100); }
    assert(one_ema_update(&ema, 100) == 100);
    assert(one_vote_init(&vote, 3u, 2u));
    assert(!one_vote_push(&vote, true));
    assert(one_vote_push(&vote, true));
    assert(one_vote_push(&vote, false));
    assert(!one_vote_push(&vote, false));
    one_vote_reset(&vote);
    assert(!one_vote_get(&vote));
    assert(!one_vote_init(&vote, 0u, 1u));
    assert(!one_vote_init(&vote, 33u, 1u));
    assert(!one_vote_init(&vote, 2u, 3u));
    assert(one_vote_init(&vote, 32u, 32u));
    for (unsigned i = 0u; i < 31u; ++i) assert(!one_vote_push(&vote, true));
    assert(one_vote_push(&vote, true));
    assert(!one_vote_push(&vote, false));
    assert(one_stale_check(10u, 0u, 9u) == ONE_STALE_STALE);
    assert(one_stale_check(10u, 0u, 10u) == ONE_STALE_FRESH);
    assert(one_stale_check(0u, UINT32_MAX, 1u) == ONE_STALE_FRESH);
    one_outlier_init(&outlier, INT32_MIN, UINT32_MAX);
    assert(one_outlier_accept(&outlier, INT32_MAX));
    one_ema_init(&ema, INT32_MIN, 31u);
    assert(one_ema_update(&ema, INT32_MAX) <= INT32_MAX);
    test_outlier_exhaustive();
    test_vote_exhaustive();
    test_stale_wrap_boundaries();
    test_ema_all_shifts();
    return 0;
}
