#include "one/seq.h"

static bool one_seq_is_valid(const one_seq_t *seq)
{
    uint64_t expected_capacity;
    if (seq == 0 || seq->min_value > seq->max_value) return false;
    expected_capacity = (uint64_t)seq->max_value - (uint64_t)seq->min_value + 1u;
    return seq->capacity == expected_capacity;
}

static one_seq_status_t one_seq_validate_value(const one_seq_t *seq, uint32_t value)
{
    if (!one_seq_is_valid(seq)) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    if (value < seq->min_value || value > seq->max_value) return ONE_SEQ_ERR_VALUE_OUT_OF_RANGE;
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_init_range(one_seq_t *seq, uint32_t min_value, uint32_t max_value)
{
    if (seq == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    if (min_value > max_value) return ONE_SEQ_ERR_INVALID_RANGE;
    seq->min_value = min_value;
    seq->max_value = max_value;
    seq->capacity = (uint64_t)max_value - (uint64_t)min_value + 1u;
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_init_bits(one_seq_t *seq, uint8_t bits)
{
    uint32_t max_value;
    if (seq == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    if (bits == 0u || bits > 32u) return ONE_SEQ_ERR_INVALID_BITS;
    max_value = (bits == 32u) ? UINT32_MAX : (UINT32_C(1) << bits) - 1u;
    return one_seq_init_range(seq, 0u, max_value);
}

one_seq_status_t one_seq_advance(const one_seq_t *seq, uint32_t value, uint64_t steps, uint32_t *out_value)
{
    one_seq_status_t status;
    uint64_t normalized;
    uint64_t offset;
    if (out_value == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    status = one_seq_validate_value(seq, value);
    if (status != ONE_SEQ_OK) return status;
    normalized = (uint64_t)value - (uint64_t)seq->min_value;
    offset = steps % seq->capacity;
    normalized = normalized >= seq->capacity - offset
        ? normalized - (seq->capacity - offset) : normalized + offset;
    *out_value = (uint32_t)(normalized + (uint64_t)seq->min_value);
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_next(const one_seq_t *seq, uint32_t value, uint32_t *out_value)
{
    return one_seq_advance(seq, value, 1u, out_value);
}

one_seq_status_t one_seq_prev(const one_seq_t *seq, uint32_t value, uint32_t *out_value)
{
    one_seq_status_t status;
    if (out_value == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    status = one_seq_validate_value(seq, value);
    if (status != ONE_SEQ_OK) return status;
    *out_value = value == seq->min_value ? seq->max_value : value - 1u;
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_forward_distance(const one_seq_t *seq, uint32_t from, uint32_t to, uint64_t *out_distance)
{
    one_seq_status_t status;
    uint64_t normalized_from;
    uint64_t normalized_to;
    if (out_distance == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    status = one_seq_validate_value(seq, from);
    if (status != ONE_SEQ_OK) return status;
    status = one_seq_validate_value(seq, to);
    if (status != ONE_SEQ_OK) return status;
    normalized_from = (uint64_t)from - (uint64_t)seq->min_value;
    normalized_to = (uint64_t)to - (uint64_t)seq->min_value;
    *out_distance = normalized_to >= normalized_from ? normalized_to - normalized_from
        : seq->capacity - (normalized_from - normalized_to);
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_backward_distance(const one_seq_t *seq, uint32_t from, uint32_t to, uint64_t *out_distance)
{
    return one_seq_forward_distance(seq, to, from, out_distance);
}

one_seq_status_t one_seq_compare(const one_seq_t *seq, uint32_t a, uint32_t b, one_seq_order_t *out_order)
{
    one_seq_status_t status;
    uint64_t distance;
    if (out_order == 0) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    status = one_seq_forward_distance(seq, b, a, &distance);
    if (status != ONE_SEQ_OK) return status;
    if (distance == 0u) *out_order = ONE_SEQ_ORDER_EQUAL;
    else if (distance < seq->capacity - distance) *out_order = ONE_SEQ_ORDER_NEWER;
    else if (distance > seq->capacity - distance) *out_order = ONE_SEQ_ORDER_OLDER;
    else *out_order = ONE_SEQ_ORDER_AMBIGUOUS;
    return ONE_SEQ_OK;
}

one_seq_status_t one_seq_in_forward_window(const one_seq_t *seq, uint32_t base, uint32_t value, uint64_t window, bool *out_result)
{
    one_seq_status_t status;
    uint64_t distance;
    if (out_result == 0 || !one_seq_is_valid(seq)) return ONE_SEQ_ERR_INVALID_ARGUMENT;
    if (window >= seq->capacity) return ONE_SEQ_ERR_WINDOW_OUT_OF_RANGE;
    status = one_seq_forward_distance(seq, base, value, &distance);
    if (status != ONE_SEQ_OK) return status;
    *out_result = distance <= window;
    return ONE_SEQ_OK;
}
