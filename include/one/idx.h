#ifndef ONE_IDX_H
#define ONE_IDX_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_idx_status {
    ONEIDX_OK = 0,
    ONEIDX_ERR_INVALID_ARGUMENT,
    ONEIDX_ERR_INVALID_RANGE,
    ONEIDX_ERR_INVALID_SCHEMA,
    ONEIDX_ERR_DUPLICATE_SYMBOL,
    ONEIDX_ERR_INVALID_KEY,
    ONEIDX_ERR_WRONG_MODE,
    ONEIDX_ERR_OVERFLOW
} one_idx_status_t;

typedef struct one_idx_position {
    const uint8_t *symbols;
    size_t count;
} one_idx_position_t;

#define ONEIDX_CHARS(s) { (const uint8_t *)(s), sizeof(s) - 1u }

typedef struct one_idx {
    uint8_t mode;
    uint32_t min_value;
    size_t capacity;
    const one_idx_position_t *positions;
    size_t position_count;
} one_idx_t;

one_idx_status_t one_idx_init_range(one_idx_t *idx, uint32_t min_value, uint32_t max_value);
one_idx_status_t one_idx_init_symbols(one_idx_t *idx, const one_idx_position_t *positions, size_t position_count);
one_idx_status_t one_idx_index_u32(const one_idx_t *idx, uint32_t value, size_t *out_index);
one_idx_status_t one_idx_index(const one_idx_t *idx, const uint8_t *key, size_t *out_index);
size_t one_idx_capacity(const one_idx_t *idx);

#ifdef __cplusplus
}
#endif

#endif
