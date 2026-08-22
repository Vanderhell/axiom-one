#include "one/scale.h"

static uint64_t one_scale_offset(uint64_t input_offset, uint64_t input_span, uint64_t output_span)
{
    uint64_t numerator = input_offset * output_span;
    uint64_t quotient = numerator / input_span;
    uint64_t remainder = numerator % input_span;
    return remainder != 0u && remainder >= input_span - remainder ? quotient + 1u : quotient;
}

static one_scale_status_t one_scale_u32_impl(uint32_t value, uint32_t in_min, uint32_t in_max,
                                              uint32_t out_min, uint32_t out_max, uint32_t *out, int clamp)
{
    uint64_t offset;
    uint64_t span;
    uint64_t output_span;
    uint32_t effective;
    if (out == 0) return ONESCALE_ERR_INVALID_ARGUMENT;
    if (in_min >= in_max) return ONESCALE_ERR_INVALID_RANGE;
    if (!clamp && (value < in_min || value > in_max)) return ONESCALE_ERR_OUT_OF_RANGE;
    effective = value < in_min ? in_min : (value > in_max ? in_max : value);
    span = (uint64_t)in_max - in_min;
    output_span = out_max >= out_min ? (uint64_t)out_max - out_min : (uint64_t)out_min - out_max;
    offset = one_scale_offset((uint64_t)effective - in_min, span, output_span);
    *out = out_max >= out_min ? (uint32_t)((uint64_t)out_min + offset) : (uint32_t)((uint64_t)out_min - offset);
    return ONESCALE_OK;
}

static one_scale_status_t one_scale_i32_impl(int32_t value, int32_t in_min, int32_t in_max,
                                              int32_t out_min, int32_t out_max, int32_t *out, int clamp)
{
    int64_t effective;
    uint64_t offset;
    uint64_t span;
    uint64_t output_span;
    if (out == 0) return ONESCALE_ERR_INVALID_ARGUMENT;
    if (in_min >= in_max) return ONESCALE_ERR_INVALID_RANGE;
    if (!clamp && (value < in_min || value > in_max)) return ONESCALE_ERR_OUT_OF_RANGE;
    effective = value < in_min ? in_min : (value > in_max ? in_max : value);
    span = (uint64_t)((int64_t)in_max - in_min);
    output_span = out_max >= out_min ? (uint64_t)((int64_t)out_max - out_min) : (uint64_t)((int64_t)out_min - out_max);
    offset = one_scale_offset((uint64_t)(effective - in_min), span, output_span);
    *out = (int32_t)(out_max >= out_min ? (int64_t)out_min + (int64_t)offset : (int64_t)out_min - (int64_t)offset);
    return ONESCALE_OK;
}

one_scale_status_t one_scale_u32(uint32_t value, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max, uint32_t *out) { return one_scale_u32_impl(value, in_min, in_max, out_min, out_max, out, 0); }
one_scale_status_t one_scale_i32(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, int32_t *out) { return one_scale_i32_impl(value, in_min, in_max, out_min, out_max, out, 0); }
one_scale_status_t one_scale_u32_clamp(uint32_t value, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max, uint32_t *out) { return one_scale_u32_impl(value, in_min, in_max, out_min, out_max, out, 1); }
one_scale_status_t one_scale_i32_clamp(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, int32_t *out) { return one_scale_i32_impl(value, in_min, in_max, out_min, out_max, out, 1); }
