#include "test_assert.h"
#include <stdint.h>

#include "one/scale.h"

static int32_t reference_scale(int32_t value, int32_t in_min, int32_t in_max,
                               int32_t out_min, int32_t out_max)
{
    uint64_t input_offset = (uint64_t)((int64_t)value - in_min);
    uint64_t input_span = (uint64_t)((int64_t)in_max - in_min);
    uint64_t output_span = out_max >= out_min
        ? (uint64_t)((int64_t)out_max - out_min)
        : (uint64_t)((int64_t)out_min - out_max);
    uint64_t numerator = input_offset * output_span;
    uint64_t offset = numerator / input_span;
    uint64_t remainder = numerator % input_span;

    if (remainder != 0u && remainder >= input_span - remainder) ++offset;
    return (int32_t)(out_max >= out_min ? (int64_t)out_min + (int64_t)offset
                                            : (int64_t)out_min - (int64_t)offset);
}

int main(void)
{
    uint32_t u = 77u;
    int32_t i = 77;
    assert(one_scale_u32(25u, 0u, 100u, 0u, 1000u, &u) == ONESCALE_OK && u == 250u);
    assert(one_scale_u32(0u, 0u, 100u, 1000u, 0u, &u) == ONESCALE_OK && u == 1000u);
    assert(one_scale_u32(100u, 0u, 100u, 1000u, 0u, &u) == ONESCALE_OK && u == 0u);
    assert(one_scale_u32(1u, 0u, 2u, 0u, 1u, &u) == ONESCALE_OK && u == 1u);
    assert(one_scale_u32(1u, 0u, 2u, 10u, 9u, &u) == ONESCALE_OK && u == 9u);
    assert(one_scale_u32(101u, 0u, 100u, 0u, 1u, &u) == ONESCALE_ERR_OUT_OF_RANGE && u == 9u);
    assert(one_scale_u32_clamp(101u, 0u, 100u, 0u, 1u, &u) == ONESCALE_OK && u == 1u);
    assert(one_scale_u32(0u, 1u, 1u, 0u, 1u, &u) == ONESCALE_ERR_INVALID_RANGE && u == 1u);
    assert(one_scale_u32(0u, 0u, 1u, 0u, 1u, 0) == ONESCALE_ERR_INVALID_ARGUMENT);
    assert(one_scale_u32_clamp(0u, 1u, 1u, 0u, 1u, &u) == ONESCALE_ERR_INVALID_RANGE);
    assert(one_scale_u32(UINT32_C(123456789), 0u, UINT32_MAX, 0u, UINT32_MAX, &u) == ONESCALE_OK && u == UINT32_C(123456789));
    assert(one_scale_u32(0u, 0u, UINT32_MAX, UINT32_MAX, 0u, &u) == ONESCALE_OK && u == UINT32_MAX);

    assert(one_scale_i32(0, -100, 100, -1000, 1000, &i) == ONESCALE_OK && i == 0);
    assert(one_scale_i32(1, 0, 2, -10, -9, &i) == ONESCALE_OK && i == -9);
    assert(one_scale_i32(1, 0, 2, -9, -10, &i) == ONESCALE_OK && i == -10);
    assert(one_scale_i32_clamp(-101, -100, 100, 0, 1000, &i) == ONESCALE_OK && i == 0);
    assert(one_scale_i32_clamp(101, -100, 100, 0, 1000, &i) == ONESCALE_OK && i == 1000);
    assert(one_scale_i32(INT32_MIN, INT32_MIN, INT32_MAX, INT32_MIN, INT32_MAX, &i) == ONESCALE_OK && i == INT32_MIN);
    assert(one_scale_i32(INT32_MAX, INT32_MIN, INT32_MAX, INT32_MAX, INT32_MIN, &i) == ONESCALE_OK && i == INT32_MIN);
    assert(one_scale_i32(0, 0, 1, 0, 1, 0) == ONESCALE_ERR_INVALID_ARGUMENT);
    assert(one_scale_i32(2, 0, 1, 0, 1, &i) == ONESCALE_ERR_OUT_OF_RANGE);
    for (int32_t in_min = -4; in_min <= 3; ++in_min) {
        for (int32_t in_max = in_min + 1; in_max <= 4; ++in_max) {
            for (int32_t out_min = -4; out_min <= 4; ++out_min) {
                for (int32_t out_max = -4; out_max <= 4; ++out_max) {
                    for (int32_t value = in_min; value <= in_max; ++value) {
                        assert(one_scale_i32(value, in_min, in_max, out_min, out_max, &i) == ONESCALE_OK);
                        assert(i == reference_scale(value, in_min, in_max, out_min, out_max));
                        assert(i >= (out_min < out_max ? out_min : out_max));
                        assert(i <= (out_min > out_max ? out_min : out_max));
                    }
                }
            }
        }
    }
    return 0;
}
