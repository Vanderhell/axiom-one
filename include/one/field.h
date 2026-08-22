#ifndef ONE_FIELD_H
#define ONE_FIELD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ONEFIELD_BITS_1  1u
#define ONEFIELD_BITS_2  2u
#define ONEFIELD_BITS_4  4u
#define ONEFIELD_BITS_8  8u
#define ONEFIELD_BITS_16 16u
#define ONEFIELD_BITS_32 32u

typedef enum one_field_status {
    ONEFIELD_OK = 0,
    ONEFIELD_ERR_INVALID_ARGUMENT,
    ONEFIELD_ERR_INVALID_RANGE,
    ONEFIELD_ERR_INVALID_SCHEMA,
    ONEFIELD_ERR_DUPLICATE_SYMBOL,
    ONEFIELD_ERR_INVALID_KEY,
    ONEFIELD_ERR_INVALID_WIDTH,
    ONEFIELD_ERR_VALUE_OUT_OF_RANGE,
    ONEFIELD_ERR_STORAGE_TOO_SMALL,
    ONEFIELD_ERR_WRONG_MODE,
    ONEFIELD_ERR_INDEX_OUT_OF_RANGE,
    ONEFIELD_ERR_OVERFLOW
} one_field_status_t;

typedef struct one_field_position {
    const uint8_t *symbols;
    size_t count;
} one_field_position_t;

#define ONEFIELD_CHARS(s) { (const uint8_t *)(s), sizeof(s) - 1u }

typedef struct one_field {
    uint8_t mode;
    uint8_t bits_per_value;
    uint32_t min_value;
    size_t capacity;
    uint8_t *storage;
    size_t storage_size;
    const one_field_position_t *positions;
    size_t position_count;
} one_field_t;

one_field_status_t one_field_required_bytes_range(uint32_t min_value, uint32_t max_value,
                                                  uint8_t bits_per_value, size_t *out_bytes);
one_field_status_t one_field_required_bytes_symbols(const one_field_position_t *positions,
                                                    size_t position_count, uint8_t bits_per_value,
                                                    size_t *out_bytes);
one_field_status_t one_field_init_range(one_field_t *field, uint32_t min_value, uint32_t max_value,
                                        uint8_t bits_per_value, void *storage, size_t storage_size);
one_field_status_t one_field_init_symbols(one_field_t *field, const one_field_position_t *positions,
                                          size_t position_count, uint8_t bits_per_value,
                                          void *storage, size_t storage_size);
one_field_status_t one_field_get_u32(const one_field_t *field, uint32_t id, uint32_t *out_value);
one_field_status_t one_field_set_u32(one_field_t *field, uint32_t id, uint32_t value);
one_field_status_t one_field_get(const one_field_t *field, const uint8_t *key, uint32_t *out_value);
one_field_status_t one_field_set(one_field_t *field, const uint8_t *key, uint32_t value);
one_field_status_t one_field_get_index(const one_field_t *field, size_t index, uint32_t *out_value);
one_field_status_t one_field_set_index(one_field_t *field, size_t index, uint32_t value);
void one_field_clear(one_field_t *field);
one_field_status_t one_field_fill(one_field_t *field, uint32_t value);

#ifdef __cplusplus
}
#endif

#endif
