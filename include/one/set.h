#ifndef ONE_SET_H
#define ONE_SET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_set_status { ONESET_OK = 0, ONESET_ERR_INVALID_ARGUMENT, ONESET_ERR_INVALID_RANGE, ONESET_ERR_INVALID_SCHEMA, ONESET_ERR_DUPLICATE_SYMBOL, ONESET_ERR_INVALID_KEY, ONESET_ERR_STORAGE_TOO_SMALL, ONESET_ERR_WRONG_MODE, ONESET_ERR_OVERFLOW } one_set_status_t;
typedef struct one_set_position { const uint8_t *symbols; size_t count; } one_set_position_t;
#define ONESET_CHARS(s) { (const uint8_t *)(s), sizeof(s) - 1u }
typedef struct one_set { uint8_t mode; uint32_t min_value; size_t capacity; uint8_t *storage; size_t storage_size; const one_set_position_t *positions; size_t position_count; } one_set_t;

one_set_status_t one_set_required_bytes_range(uint32_t min_value, uint32_t max_value, size_t *out_bytes);
one_set_status_t one_set_required_bytes_symbols(const one_set_position_t *positions, size_t position_count, size_t *out_bytes);
one_set_status_t one_set_init_range(one_set_t *set, uint32_t min_value, uint32_t max_value, void *storage, size_t storage_size);
one_set_status_t one_set_init_symbols(one_set_t *set, const one_set_position_t *positions, size_t position_count, void *storage, size_t storage_size);
one_set_status_t one_set_index_u32(const one_set_t *set, uint32_t value, size_t *out_index);
one_set_status_t one_set_index(const one_set_t *set, const uint8_t *key, size_t *out_index);
bool one_set_contains_u32(const one_set_t *set, uint32_t value);
bool one_set_contains(const one_set_t *set, const uint8_t *key);
one_set_status_t one_set_contains_u32_checked(const one_set_t *set, uint32_t value, bool *contains);
one_set_status_t one_set_contains_checked(const one_set_t *set, const uint8_t *key, bool *contains);
one_set_status_t one_set_add_u32(one_set_t *set, uint32_t value);
one_set_status_t one_set_remove_u32(one_set_t *set, uint32_t value);
one_set_status_t one_set_add(one_set_t *set, const uint8_t *key);
one_set_status_t one_set_remove(one_set_t *set, const uint8_t *key);
bool one_set_contains_index(const one_set_t *set, size_t index);
void one_set_add_index(one_set_t *set, size_t index);
void one_set_remove_index(one_set_t *set, size_t index);
void one_set_clear(one_set_t *set);
void one_set_fill(one_set_t *set);

#ifdef __cplusplus
}
#endif
#endif
