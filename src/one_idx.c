#include "one/idx.h"

enum { ONE_IDX_RANGE = 1, ONE_IDX_SYMBOLS = 2 };

static one_idx_status_t one_idx_validate_schema(const one_idx_position_t *positions,
                                                size_t position_count, size_t *out_capacity)
{
    size_t i;
    size_t capacity = 1u;
    if (positions == 0 || out_capacity == 0 || position_count == 0u) return ONEIDX_ERR_INVALID_ARGUMENT;
    for (i = 0u; i < position_count; ++i) {
        size_t a;
        size_t b;
        if (positions[i].symbols == 0 || positions[i].count == 0u) return ONEIDX_ERR_INVALID_SCHEMA;
        for (a = 0u; a < positions[i].count; ++a)
            for (b = a + 1u; b < positions[i].count; ++b)
                if (positions[i].symbols[a] == positions[i].symbols[b]) return ONEIDX_ERR_DUPLICATE_SYMBOL;
        if (capacity > SIZE_MAX / positions[i].count) return ONEIDX_ERR_OVERFLOW;
        capacity *= positions[i].count;
    }
    *out_capacity = capacity;
    return ONEIDX_OK;
}

one_idx_status_t one_idx_init_range(one_idx_t *idx, uint32_t min_value, uint32_t max_value)
{
    uint64_t capacity;
    if (idx == 0) return ONEIDX_ERR_INVALID_ARGUMENT;
    if (min_value > max_value) return ONEIDX_ERR_INVALID_RANGE;
    capacity = (uint64_t)max_value - min_value + 1u;
    if (capacity > SIZE_MAX) return ONEIDX_ERR_OVERFLOW;
    idx->mode = ONE_IDX_RANGE;
    idx->min_value = min_value;
    idx->capacity = (size_t)capacity;
    idx->positions = 0;
    idx->position_count = 0u;
    return ONEIDX_OK;
}

one_idx_status_t one_idx_init_symbols(one_idx_t *idx, const one_idx_position_t *positions, size_t position_count)
{
    size_t capacity;
    one_idx_status_t status;
    if (idx == 0) return ONEIDX_ERR_INVALID_ARGUMENT;
    status = one_idx_validate_schema(positions, position_count, &capacity);
    if (status != ONEIDX_OK) return status;
    idx->mode = ONE_IDX_SYMBOLS;
    idx->min_value = 0u;
    idx->capacity = capacity;
    idx->positions = positions;
    idx->position_count = position_count;
    return ONEIDX_OK;
}

one_idx_status_t one_idx_index_u32(const one_idx_t *idx, uint32_t value, size_t *out_index)
{
    if (idx == 0 || out_index == 0) return ONEIDX_ERR_INVALID_ARGUMENT;
    if (idx->mode != ONE_IDX_RANGE) return ONEIDX_ERR_WRONG_MODE;
    if (value < idx->min_value || (uint64_t)value - idx->min_value >= idx->capacity) return ONEIDX_ERR_INVALID_KEY;
    *out_index = (size_t)((uint64_t)value - idx->min_value);
    return ONEIDX_OK;
}

one_idx_status_t one_idx_index(const one_idx_t *idx, const uint8_t *key, size_t *out_index)
{
    size_t i;
    size_t index = 0u;
    if (idx == 0 || key == 0 || out_index == 0) return ONEIDX_ERR_INVALID_ARGUMENT;
    if (idx->mode != ONE_IDX_SYMBOLS) return ONEIDX_ERR_WRONG_MODE;
    for (i = 0u; i < idx->position_count; ++i) {
        size_t ordinal;
        for (ordinal = 0u; ordinal < idx->positions[i].count; ++ordinal)
            if (idx->positions[i].symbols[ordinal] == key[i]) break;
        if (ordinal == idx->positions[i].count) return ONEIDX_ERR_INVALID_KEY;
        index = index * idx->positions[i].count + ordinal;
    }
    *out_index = index;
    return ONEIDX_OK;
}

size_t one_idx_capacity(const one_idx_t *idx)
{
    return (idx != 0 && (idx->mode == ONE_IDX_RANGE || idx->mode == ONE_IDX_SYMBOLS)) ? idx->capacity : 0u;
}
