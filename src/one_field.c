#include "one/field.h"

#include <stdint.h>

enum { ONE_FIELD_RANGE = 1, ONE_FIELD_SYMBOLS = 2 };

static int one_field_width_valid(uint8_t bits)
{
    return bits == 1u || bits == 2u || bits == 4u || bits == 8u || bits == 16u || bits == 32u;
}

static one_field_status_t one_field_bytes(size_t capacity, uint8_t bits, size_t *out_bytes)
{
    size_t total_bits;
    if (out_bytes == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (!one_field_width_valid(bits)) return ONEFIELD_ERR_INVALID_WIDTH;
    if (capacity > (SIZE_MAX - 7u) / (size_t)bits) return ONEFIELD_ERR_OVERFLOW;
    total_bits = capacity * (size_t)bits;
    *out_bytes = (total_bits + 7u) / 8u;
    return ONEFIELD_OK;
}

static one_field_status_t one_field_symbols_capacity(const one_field_position_t *positions,
                                                      size_t position_count, size_t *out_capacity)
{
    size_t i;
    size_t capacity = 1u;
    if (positions == 0 || out_capacity == 0 || position_count == 0u) return ONEFIELD_ERR_INVALID_ARGUMENT;
    for (i = 0u; i < position_count; ++i) {
        size_t a;
        size_t b;
        if (positions[i].symbols == 0 || positions[i].count == 0u) return ONEFIELD_ERR_INVALID_SCHEMA;
        for (a = 0u; a < positions[i].count; ++a)
            for (b = a + 1u; b < positions[i].count; ++b)
                if (positions[i].symbols[a] == positions[i].symbols[b]) return ONEFIELD_ERR_DUPLICATE_SYMBOL;
        if (capacity > SIZE_MAX / positions[i].count) return ONEFIELD_ERR_OVERFLOW;
        capacity *= positions[i].count;
    }
    *out_capacity = capacity;
    return ONEFIELD_OK;
}

static int one_field_valid(const one_field_t *field)
{
    size_t needed;
    return field != 0 && (field->mode == ONE_FIELD_RANGE || field->mode == ONE_FIELD_SYMBOLS) &&
        one_field_bytes(field->capacity, field->bits_per_value, &needed) == ONEFIELD_OK &&
        field->storage != 0 && field->storage_size >= needed;
}

one_field_status_t one_field_required_bytes_range(uint32_t min_value, uint32_t max_value,
                                                  uint8_t bits_per_value, size_t *out_bytes)
{
    uint64_t capacity;
    if (min_value > max_value) return ONEFIELD_ERR_INVALID_RANGE;
    capacity = (uint64_t)max_value - min_value + 1u;
    if (capacity > SIZE_MAX) return ONEFIELD_ERR_OVERFLOW;
    return one_field_bytes((size_t)capacity, bits_per_value, out_bytes);
}

one_field_status_t one_field_required_bytes_symbols(const one_field_position_t *positions,
                                                    size_t position_count, uint8_t bits_per_value,
                                                    size_t *out_bytes)
{
    size_t capacity;
    one_field_status_t status = one_field_symbols_capacity(positions, position_count, &capacity);
    return status == ONEFIELD_OK ? one_field_bytes(capacity, bits_per_value, out_bytes) : status;
}

one_field_status_t one_field_init_range(one_field_t *field, uint32_t min_value, uint32_t max_value,
                                        uint8_t bits_per_value, void *storage, size_t storage_size)
{
    size_t needed;
    one_field_status_t status;
    if (field == 0 || storage == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    status = one_field_required_bytes_range(min_value, max_value, bits_per_value, &needed);
    if (status != ONEFIELD_OK) return status;
    if (storage_size < needed) return ONEFIELD_ERR_STORAGE_TOO_SMALL;
    field->mode = ONE_FIELD_RANGE; field->bits_per_value = bits_per_value; field->min_value = min_value;
    field->capacity = (size_t)((uint64_t)max_value - min_value + 1u); field->storage = storage;
    field->storage_size = storage_size; field->positions = 0; field->position_count = 0u;
    return ONEFIELD_OK;
}

one_field_status_t one_field_init_symbols(one_field_t *field, const one_field_position_t *positions,
                                          size_t position_count, uint8_t bits_per_value,
                                          void *storage, size_t storage_size)
{
    size_t capacity; size_t needed; one_field_status_t status;
    if (field == 0 || storage == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    status = one_field_symbols_capacity(positions, position_count, &capacity);
    if (status != ONEFIELD_OK) return status;
    status = one_field_bytes(capacity, bits_per_value, &needed);
    if (status != ONEFIELD_OK) return status;
    if (storage_size < needed) return ONEFIELD_ERR_STORAGE_TOO_SMALL;
    field->mode = ONE_FIELD_SYMBOLS; field->bits_per_value = bits_per_value; field->min_value = 0u;
    field->capacity = capacity; field->storage = storage; field->storage_size = storage_size;
    field->positions = positions; field->position_count = position_count;
    return ONEFIELD_OK;
}

one_field_status_t one_field_get_index(const one_field_t *field, size_t index, uint32_t *out_value)
{
    size_t offset; uint8_t shift; uint8_t mask;
    if (out_value == 0 || !one_field_valid(field)) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (index >= field->capacity) return ONEFIELD_ERR_INDEX_OUT_OF_RANGE;
    if (field->bits_per_value < 8u) {
        offset = (index * field->bits_per_value) >> 3; shift = (uint8_t)((index * field->bits_per_value) & 7u);
        mask = (uint8_t)((UINT32_C(1) << field->bits_per_value) - 1u);
        *out_value = (field->storage[offset] >> shift) & mask;
    } else if (field->bits_per_value == 8u) *out_value = field->storage[index];
    else if (field->bits_per_value == 16u) { offset = index * 2u; *out_value = field->storage[offset] | ((uint32_t)field->storage[offset + 1u] << 8u); }
    else { offset = index * 4u; *out_value = field->storage[offset] | ((uint32_t)field->storage[offset + 1u] << 8u) | ((uint32_t)field->storage[offset + 2u] << 16u) | ((uint32_t)field->storage[offset + 3u] << 24u); }
    return ONEFIELD_OK;
}

one_field_status_t one_field_set_index(one_field_t *field, size_t index, uint32_t value)
{
    size_t offset; uint8_t shift; uint8_t mask;
    if (!one_field_valid(field)) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (index >= field->capacity) return ONEFIELD_ERR_INDEX_OUT_OF_RANGE;
    if (field->bits_per_value < 32u && value >= (UINT32_C(1) << field->bits_per_value)) return ONEFIELD_ERR_VALUE_OUT_OF_RANGE;
    if (field->bits_per_value < 8u) {
        offset = (index * field->bits_per_value) >> 3; shift = (uint8_t)((index * field->bits_per_value) & 7u);
        mask = (uint8_t)(((UINT32_C(1) << field->bits_per_value) - 1u) << shift);
        field->storage[offset] = (uint8_t)((field->storage[offset] & (uint8_t)~mask) | ((uint8_t)(value << shift) & mask));
    } else if (field->bits_per_value == 8u) field->storage[index] = (uint8_t)value;
    else if (field->bits_per_value == 16u) { offset = index * 2u; field->storage[offset] = (uint8_t)value; field->storage[offset + 1u] = (uint8_t)(value >> 8u); }
    else { offset = index * 4u; field->storage[offset] = (uint8_t)value; field->storage[offset + 1u] = (uint8_t)(value >> 8u); field->storage[offset + 2u] = (uint8_t)(value >> 16u); field->storage[offset + 3u] = (uint8_t)(value >> 24u); }
    return ONEFIELD_OK;
}

one_field_status_t one_field_get_u32(const one_field_t *field, uint32_t id, uint32_t *out_value)
{
    if (!one_field_valid(field) || out_value == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (field->mode != ONE_FIELD_RANGE) return ONEFIELD_ERR_WRONG_MODE;
    if (id < field->min_value || (uint64_t)id - field->min_value >= field->capacity) return ONEFIELD_ERR_INDEX_OUT_OF_RANGE;
    return one_field_get_index(field, (size_t)((uint64_t)id - field->min_value), out_value);
}

one_field_status_t one_field_set_u32(one_field_t *field, uint32_t id, uint32_t value)
{
    if (!one_field_valid(field)) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (field->mode != ONE_FIELD_RANGE) return ONEFIELD_ERR_WRONG_MODE;
    if (id < field->min_value || (uint64_t)id - field->min_value >= field->capacity) return ONEFIELD_ERR_INDEX_OUT_OF_RANGE;
    return one_field_set_index(field, (size_t)((uint64_t)id - field->min_value), value);
}

static one_field_status_t one_field_key_index(const one_field_t *field, const uint8_t *key, size_t *out_index)
{
    size_t i; size_t index = 0u;
    if (key == 0 || out_index == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    for (i = 0u; i < field->position_count; ++i) { size_t ordinal; for (ordinal = 0u; ordinal < field->positions[i].count && field->positions[i].symbols[ordinal] != key[i]; ++ordinal) {} if (ordinal == field->positions[i].count) return ONEFIELD_ERR_INVALID_KEY; index = index * field->positions[i].count + ordinal; }
    *out_index = index; return ONEFIELD_OK;
}

one_field_status_t one_field_get(const one_field_t *field, const uint8_t *key, uint32_t *out_value)
{
    size_t index; one_field_status_t status;
    if (!one_field_valid(field) || out_value == 0) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (field->mode != ONE_FIELD_SYMBOLS) return ONEFIELD_ERR_WRONG_MODE;
    status = one_field_key_index(field, key, &index); return status == ONEFIELD_OK ? one_field_get_index(field, index, out_value) : status;
}

one_field_status_t one_field_set(one_field_t *field, const uint8_t *key, uint32_t value)
{
    size_t index; one_field_status_t status;
    if (!one_field_valid(field)) return ONEFIELD_ERR_INVALID_ARGUMENT;
    if (field->mode != ONE_FIELD_SYMBOLS) return ONEFIELD_ERR_WRONG_MODE;
    status = one_field_key_index(field, key, &index); return status == ONEFIELD_OK ? one_field_set_index(field, index, value) : status;
}

void one_field_clear(one_field_t *field)
{
    size_t i; size_t needed;
    if (!one_field_valid(field) || one_field_bytes(field->capacity, field->bits_per_value, &needed) != ONEFIELD_OK) return;
    for (i = 0u; i < needed; ++i) field->storage[i] = 0u;
}

one_field_status_t one_field_fill(one_field_t *field, uint32_t value)
{
    size_t i; one_field_status_t status;
    if (!one_field_valid(field)) return ONEFIELD_ERR_INVALID_ARGUMENT;
    for (i = 0u; i < field->capacity; ++i) { status = one_field_set_index(field, i, value); if (status != ONEFIELD_OK) return status; }
    return ONEFIELD_OK;
}
