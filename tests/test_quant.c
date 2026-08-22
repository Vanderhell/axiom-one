#include "test_assert.h"
#include <stdint.h>

#include "one/quant.h"

static int32_t reference_quant(int32_t value, int32_t origin, uint32_t step)
{
    int32_t candidate;
    int32_t best = origin;
    int64_t best_distance = INT64_MAX;

    for (candidate = -256; candidate <= 255; ++candidate) {
        int64_t delta = (int64_t)candidate - origin;
        int64_t distance = (int64_t)candidate - value;

        if (delta % (int64_t)step != 0) continue;
        if (distance < 0) distance = -distance;
        if (distance < best_distance ||
            (distance == best_distance && (delta < 0 ? -delta : delta) >
                                       ((int64_t)best - origin < 0 ? -((int64_t)best - origin) : (int64_t)best - origin))) {
            best = candidate;
            best_distance = distance;
        }
    }
    return best;
}

int main(void)
{
    int32_t signed_out = 77;
    uint32_t unsigned_out = 77u;
    assert(one_quant_u32(237u, 0u, 25u, &unsigned_out) == ONEQUANT_OK && unsigned_out == 225u);
    assert(one_quant_u32(243u, 0u, 25u, &unsigned_out) == ONEQUANT_OK && unsigned_out == 250u);
    assert(one_quant_u32(5u, 0u, 10u, &unsigned_out) == ONEQUANT_OK && unsigned_out == 10u);
    assert(one_quant_u32(5u, 100u, 20u, &unsigned_out) == ONEQUANT_OK && unsigned_out == 0u);
    assert(one_quant_u32(UINT32_MAX, 0u, 10u, &unsigned_out) == ONEQUANT_OK && unsigned_out == UINT32_C(4294967290));
    assert(one_quant_u32(123u, 7u, 1u, &unsigned_out) == ONEQUANT_OK && unsigned_out == 123u);
    assert(one_quant_u32(1u, 0u, 0u, &unsigned_out) == ONEQUANT_ERR_INVALID_STEP && unsigned_out == 123u);
    assert(one_quant_u32(1u, 0u, 1u, 0) == ONEQUANT_ERR_INVALID_ARGUMENT);
    assert(one_quant_i32(1, 0, 1u, 0) == ONEQUANT_ERR_INVALID_ARGUMENT);

    assert(one_quant_i32(-37, 0, 10u, &signed_out) == ONEQUANT_OK && signed_out == -40);
    assert(one_quant_i32(-5, 0, 10u, &signed_out) == ONEQUANT_OK && signed_out == -10);
    assert(one_quant_i32(90, 100, 20u, &signed_out) == ONEQUANT_OK && signed_out == 80);
    assert(one_quant_i32(110, 100, 20u, &signed_out) == ONEQUANT_OK && signed_out == 120);
    assert(one_quant_i32(-11, -3, 8u, &signed_out) == ONEQUANT_OK && signed_out == -11);
    assert(one_quant_i32(INT32_MIN, 0, 10u, &signed_out) == ONEQUANT_OK && signed_out == INT32_C(-2147483640));
    assert(one_quant_i32(INT32_MAX, INT32_MAX, UINT32_MAX, &signed_out) == ONEQUANT_OK && signed_out == INT32_MAX);
    assert(one_quant_i32(1, 0, 0u, &signed_out) == ONEQUANT_ERR_INVALID_STEP && signed_out == INT32_MAX);
    for (int32_t origin = -16; origin <= 15; ++origin) {
        for (uint32_t step = 1u; step <= 16u; ++step) {
            for (int32_t value = -32; value <= 31; ++value) {
                assert(one_quant_i32(value, origin, step, &signed_out) == ONEQUANT_OK);
                assert(signed_out == reference_quant(value, origin, step));
                assert(one_quant_i32(signed_out, origin, step, &signed_out) == ONEQUANT_OK);
            }
        }
    }
    for (uint32_t origin = 0u; origin <= 31u; ++origin) {
        for (uint32_t step = 1u; step <= 16u; ++step) {
            for (uint32_t value = 0u; value <= 63u; ++value) {
                uint64_t distance = value >= origin ? value - origin : origin - value;
                uint64_t quotient = distance / step;
                uint64_t remainder = distance % step;
                uint64_t offset = quotient * step;
                uint32_t expected = value >= origin ? origin + (uint32_t)offset : origin - (uint32_t)offset;
                if (remainder != 0u && remainder >= step - remainder &&
                    (value >= origin ? UINT32_MAX - expected >= step : expected >= step))
                    expected = value >= origin ? expected + step : expected - step;
                assert(one_quant_u32(value, origin, step, &unsigned_out) == ONEQUANT_OK && unsigned_out == expected);
            }
        }
    }
    return 0;
}
