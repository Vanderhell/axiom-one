#include "test_assert.h"
#include <stddef.h>
#include <stdint.h>
#include "one/dead.h"
#include "one/hyst.h"
#include "one/rate.h"
#include "one/slew.h"

static uint32_t slew_u32_reference(uint32_t target, uint32_t rise, uint32_t fall, uint32_t current)
{
    if (target > current) {
        uint64_t distance = (uint64_t)target - current;
        return distance <= rise ? target : (uint32_t)((uint64_t)current + rise);
    }
    if (target < current) {
        uint64_t distance = (uint64_t)current - target;
        return distance <= fall ? target : (uint32_t)((uint64_t)current - fall);
    }
    return current;
}

static int32_t slew_i32_reference(int32_t target, uint32_t rise, uint32_t fall, int32_t current)
{
    int64_t delta = (int64_t)target - current;

    if (delta > 0) return (uint64_t)delta <= rise ? target : (int32_t)((int64_t)current + rise);
    if (delta < 0) return (uint64_t)(-delta) <= fall ? target : (int32_t)((int64_t)current - fall);
    return current;
}

static void test_slew_exhaustive(void)
{
    uint32_t target;

    for (target = 0u; target <= 32u; ++target) {
        uint32_t current;
        for (current = 0u; current <= 32u; ++current) {
            uint32_t rise;
            for (rise = 0u; rise <= 32u; ++rise) {
                uint32_t fall;
                for (fall = 0u; fall <= 32u; ++fall) {
                    uint32_t actual = current;
                    uint32_t expected = slew_u32_reference(target, rise, fall, current);
                    assert(one_slew_u32(target, rise, fall, &actual) == ONE_SLEW_OK);
                    assert(actual == expected);
                }
            }
        }
    }
    {
        int32_t target_i;
        for (target_i = -16; target_i <= 16; ++target_i) {
            int32_t current_i;
            for (current_i = -16; current_i <= 16; ++current_i) {
                uint32_t rise;
                for (rise = 0u; rise <= 32u; ++rise) {
                    uint32_t fall;
                    for (fall = 0u; fall <= 32u; ++fall) {
                        int32_t actual = current_i;
                        int32_t expected = slew_i32_reference(target_i, rise, fall, current_i);
                        assert(one_slew_i32(target_i, rise, fall, &actual) == ONE_SLEW_OK);
                        assert(actual == expected);
                    }
                }
            }
        }
    }
    {
        static const uint32_t values_u[] = { 0u, 1u, UINT32_MAX - 1u, UINT32_MAX };
        static const uint32_t limits[] = { 0u, 1u, 2u, UINT32_MAX - 1u, UINT32_MAX };
        static const int32_t values_i[] = { INT32_MIN, INT32_MIN + 1, -1, 0, 1, INT32_MAX - 1, INT32_MAX };
        size_t target_index;

        for (target_index = 0u; target_index < sizeof(values_u) / sizeof(values_u[0]); ++target_index) {
            size_t current_index;
            for (current_index = 0u; current_index < sizeof(values_u) / sizeof(values_u[0]); ++current_index) {
                size_t rise_index;
                for (rise_index = 0u; rise_index < sizeof(limits) / sizeof(limits[0]); ++rise_index) {
                    size_t fall_index;
                    for (fall_index = 0u; fall_index < sizeof(limits) / sizeof(limits[0]); ++fall_index) {
                        uint32_t actual = values_u[current_index];
                        assert(one_slew_u32(values_u[target_index], limits[rise_index], limits[fall_index], &actual) == ONE_SLEW_OK);
                        assert(actual == slew_u32_reference(values_u[target_index], limits[rise_index], limits[fall_index], values_u[current_index]));
                    }
                }
            }
        }
        for (target_index = 0u; target_index < sizeof(values_i) / sizeof(values_i[0]); ++target_index) {
            size_t current_index;
            for (current_index = 0u; current_index < sizeof(values_i) / sizeof(values_i[0]); ++current_index) {
                size_t rise_index;
                for (rise_index = 0u; rise_index < sizeof(limits) / sizeof(limits[0]); ++rise_index) {
                    size_t fall_index;
                    for (fall_index = 0u; fall_index < sizeof(limits) / sizeof(limits[0]); ++fall_index) {
                        int32_t actual = values_i[current_index];
                        assert(one_slew_i32(values_i[target_index], limits[rise_index], limits[fall_index], &actual) == ONE_SLEW_OK);
                        assert(actual == slew_i32_reference(values_i[target_index], limits[rise_index], limits[fall_index], values_i[current_index]));
                    }
                }
            }
        }
    }
    assert(one_slew_u32(0u, 0u, 0u, 0) == ONE_SLEW_ERR_INVALID_ARGUMENT);
    assert(one_slew_i32(0, 0u, 0u, 0) == ONE_SLEW_ERR_INVALID_ARGUMENT);
}

static void test_rate_exhaustive(void)
{
    uint32_t denominator;

    for (denominator = 1u; denominator <= 64u; ++denominator) {
        uint32_t numerator;
        for (numerator = 0u; numerator <= denominator; ++numerator) {
            uint32_t initial_phase;
            for (initial_phase = 0u; initial_phase < denominator; ++initial_phase) {
                uint32_t phase = initial_phase;
                uint8_t emit = 2u;
                uint64_t reference = (uint64_t)initial_phase + numerator;
                uint32_t expected_phase = (uint32_t)(reference % denominator);
                uint8_t expected_emit = (uint8_t)(reference >= denominator);

                assert(one_rate_step(numerator, denominator, &phase, &emit) == ONE_RATE_OK);
                assert(emit == expected_emit && phase == expected_phase);
            }
        }
    }
}

static void test_dead_exhaustive(void)
{
    int32_t low;

    for (low = -16; low <= 16; ++low) {
        int32_t high;
        for (high = low; high <= 16; ++high) {
            int32_t value;
            for (value = -32; value <= 32; ++value) {
                int32_t output = INT32_MIN;
                int32_t expected = value >= low && value <= high ? 0 : value;
                assert(one_dead_i32(value, low, high, &output) == ONE_DEAD_OK);
                assert(output == expected);
            }
        }
    }
    {
        uint32_t output = UINT32_MAX;
        assert(one_dead_u32(0u, 0u, UINT32_MAX, &output) == ONE_DEAD_OK && output == 0u);
        assert(one_dead_u32(UINT32_MAX, 0u, UINT32_MAX, &output) == ONE_DEAD_OK && output == 0u);
        assert(one_dead_u32(UINT32_MAX, UINT32_MAX, UINT32_MAX, &output) == ONE_DEAD_OK && output == 0u);
        output = 77u;
        assert(one_dead_u32(5u, 6u, 4u, &output) == ONE_DEAD_ERR_INVALID_RANGE && output == 77u);
        assert(one_dead_u32(5u, 0u, 9u, 0) == ONE_DEAD_ERR_INVALID_ARGUMENT);
    }
    {
        int32_t output = 77;
        assert(one_dead_i32(INT32_MIN, INT32_MIN, INT32_MAX, &output) == ONE_DEAD_OK && output == 0);
        assert(one_dead_i32(INT32_MAX, INT32_MIN, INT32_MAX, &output) == ONE_DEAD_OK && output == 0);
        output = 77;
        assert(one_dead_i32(5, 6, 4, &output) == ONE_DEAD_ERR_INVALID_RANGE && output == 77);
    }
}

static void test_hyst_exhaustive(void)
{
    int32_t low;

    for (low = -16; low < 16; ++low) {
        int32_t high;
        for (high = low + 1; high <= 16; ++high) {
            int32_t value;
            for (value = -32; value <= 32; ++value) {
                uint8_t initial;
                for (initial = 0u; initial <= 1u; ++initial) {
                    uint8_t state = initial;
                    uint8_t expected = value <= low ? 0u : (value >= high ? 1u : initial);
                    assert(one_hyst_i32(value, low, high, &state) == ONE_HYST_OK);
                    assert(state == expected);
                }
            }
        }
    }
    {
        uint32_t low_u;
        for (low_u = 0u; low_u < 16u; ++low_u) {
            uint32_t high_u;
            for (high_u = low_u + 1u; high_u <= 16u; ++high_u) {
                uint32_t value;
                for (value = 0u; value <= 32u; ++value) {
                    uint8_t initial;
                    for (initial = 0u; initial <= 1u; ++initial) {
                        uint8_t state = initial;
                        uint8_t expected = value <= low_u ? 0u : (value >= high_u ? 1u : initial);
                        assert(one_hyst_u32(value, low_u, high_u, &state) == ONE_HYST_OK);
                        assert(state == expected);
                    }
                }
            }
        }
    }
    {
        uint8_t state = 1u;
        assert(one_hyst_i32(INT32_MIN, INT32_MIN, INT32_MAX, &state) == ONE_HYST_OK && state == 0u);
        state = 0u;
        assert(one_hyst_i32(INT32_MAX, INT32_MIN, INT32_MAX, &state) == ONE_HYST_OK && state == 1u);
        state = 1u;
        assert(one_hyst_u32(4u, 5u, 5u, &state) == ONE_HYST_ERR_INVALID_RANGE && state == 1u);
        state = 2u;
        assert(one_hyst_u32(4u, 1u, 5u, &state) == ONE_HYST_ERR_INVALID_STATE && state == 2u);
    }
}

int main(void)
{
    uint32_t out = 99u, current = 10u, phase = 0u; uint8_t state = 0u, emit = 9u;
    assert(one_dead_u32(5u, 3u, 8u, &out) == ONE_DEAD_OK && out == 0u);
    assert(one_dead_u32(9u, 3u, 8u, &out) == ONE_DEAD_OK && out == 9u);
    assert(one_dead_u32(9u, 8u, 3u, &out) == ONE_DEAD_ERR_INVALID_RANGE && out == 9u);
    assert(one_hyst_u32(10u, 10u, 20u, &state) == ONE_HYST_OK && state == 0u);
    assert(one_hyst_u32(20u, 10u, 20u, &state) == ONE_HYST_OK && state == 1u);
    assert(one_hyst_u32(15u, 10u, 20u, &state) == ONE_HYST_OK && state == 1u);
    assert(one_slew_u32(30u, 5u, 4u, &current) == ONE_SLEW_OK && current == 15u);
    assert(one_slew_u32(0u, 5u, 4u, &current) == ONE_SLEW_OK && current == 11u);
    assert(one_rate_step(3u, 10u, &phase, &emit) == ONE_RATE_OK && emit == 0u && phase == 3u);
    assert(one_rate_step(3u, 10u, &phase, &emit) == ONE_RATE_OK && emit == 0u && phase == 6u);
    assert(one_rate_step(3u, 10u, &phase, &emit) == ONE_RATE_OK && emit == 0u && phase == 9u);
    assert(one_rate_step(3u, 10u, &phase, &emit) == ONE_RATE_OK && emit == 1u && phase == 2u);
    { int32_t signed_out = 99;
      assert(one_dead_i32(-5, -5, 5, &signed_out) == ONE_DEAD_OK && signed_out == 0); }
    assert(one_dead_u32(3u, 3u, 8u, &out) == ONE_DEAD_OK && out == 0u);
    assert(one_dead_u32(8u, 3u, 8u, &out) == ONE_DEAD_OK && out == 0u);
    state = 2u;
    assert(one_hyst_u32(10u, 1u, 2u, &state) == ONE_HYST_ERR_INVALID_STATE);
    state = 0u;
    assert(one_hyst_i32(-10, -5, 5, &state) == ONE_HYST_OK && state == 0u);
    assert(one_hyst_i32(5, -5, 5, &state) == ONE_HYST_OK && state == 1u);
    current = 0u;
    assert(one_slew_u32(UINT32_MAX, UINT32_MAX, 0u, &current) == ONE_SLEW_OK && current == UINT32_MAX);
    { int32_t signed_current = INT32_MIN;
      assert(one_slew_i32(INT32_MAX, UINT32_MAX, 0u, &signed_current) == ONE_SLEW_OK && signed_current == INT32_MAX); }
    phase = 0u;
    assert(one_rate_step(0u, 10u, &phase, &emit) == ONE_RATE_OK && emit == 0u && phase == 0u);
    assert(one_rate_step(11u, 10u, &phase, &emit) == ONE_RATE_ERR_INVALID_RATE);
    phase = 10u;
    assert(one_rate_step(1u, 10u, &phase, &emit) == ONE_RATE_ERR_INVALID_PHASE);
    test_rate_exhaustive();
    test_dead_exhaustive();
    test_hyst_exhaustive();
    test_slew_exhaustive();
    return 0;
}
