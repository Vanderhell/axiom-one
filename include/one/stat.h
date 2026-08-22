#ifndef ONE_STAT_H
#define ONE_STAT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_stat_status { ONESTAT_OK = 0, ONESTAT_ERR_INVALID_ARGUMENT, ONESTAT_ERR_INVALID_RANGE, ONESTAT_ERR_INVALID_SCHEMA, ONESTAT_ERR_DUPLICATE_SYMBOL, ONESTAT_ERR_INVALID_KEY, ONESTAT_ERR_INVALID_WIDTH, ONESTAT_ERR_VALUE_OUT_OF_RANGE, ONESTAT_ERR_FIELD_OUT_OF_RANGE, ONESTAT_ERR_INDEX_OUT_OF_RANGE, ONESTAT_ERR_STORAGE_TOO_SMALL, ONESTAT_ERR_WRONG_MODE, ONESTAT_ERR_OVERFLOW } one_stat_status_t;
typedef struct one_stat_position { const uint8_t *symbols; size_t count; } one_stat_position_t;
#define ONESTAT_CHARS(s) { (const uint8_t *)(s), sizeof(s) - 1u }
typedef struct one_stat { uint8_t mode; uint32_t min_value; size_t capacity; const uint8_t *field_bits; size_t field_count; size_t record_bits; uint8_t *storage; size_t storage_size; const one_stat_position_t *positions; size_t position_count; } one_stat_t;
one_stat_status_t one_stat_required_bytes_range(uint32_t min, uint32_t max, const uint8_t *bits, size_t count, size_t *out);
one_stat_status_t one_stat_required_bytes_symbols(const one_stat_position_t *positions, size_t position_count, const uint8_t *bits, size_t count, size_t *out);
one_stat_status_t one_stat_init_range(one_stat_t *s,uint32_t min,uint32_t max,const uint8_t *bits,size_t count,void *storage,size_t size);
one_stat_status_t one_stat_init_symbols(one_stat_t *s,const one_stat_position_t *positions,size_t position_count,const uint8_t *bits,size_t count,void *storage,size_t size);
one_stat_status_t one_stat_get_index(const one_stat_t*s,size_t index,size_t field,uint32_t*out);
one_stat_status_t one_stat_set_index(one_stat_t*s,size_t index,size_t field,uint32_t value);
one_stat_status_t one_stat_inc_index(one_stat_t*s,size_t index,size_t field);
one_stat_status_t one_stat_dec_index(one_stat_t*s,size_t index,size_t field);
one_stat_status_t one_stat_add_index(one_stat_t*s,size_t index,size_t field,uint32_t delta);
one_stat_status_t one_stat_reset_index(one_stat_t*s,size_t index,size_t field);
one_stat_status_t one_stat_get_u32(const one_stat_t*s,uint32_t id,size_t field,uint32_t*out);
one_stat_status_t one_stat_set_u32(one_stat_t*s,uint32_t id,size_t field,uint32_t value);
one_stat_status_t one_stat_inc_u32(one_stat_t*s,uint32_t id,size_t field);
one_stat_status_t one_stat_dec_u32(one_stat_t*s,uint32_t id,size_t field);
one_stat_status_t one_stat_add_u32(one_stat_t*s,uint32_t id,size_t field,uint32_t delta);
one_stat_status_t one_stat_reset_u32(one_stat_t*s,uint32_t id,size_t field);
one_stat_status_t one_stat_get(const one_stat_t*s,const uint8_t *key,size_t field,uint32_t*out);
one_stat_status_t one_stat_set(one_stat_t*s,const uint8_t *key,size_t field,uint32_t value);
one_stat_status_t one_stat_inc(one_stat_t*s,const uint8_t *key,size_t field);
one_stat_status_t one_stat_dec(one_stat_t*s,const uint8_t *key,size_t field);
one_stat_status_t one_stat_add(one_stat_t*s,const uint8_t *key,size_t field,uint32_t delta);
one_stat_status_t one_stat_reset(one_stat_t*s,const uint8_t *key,size_t field);
void one_stat_clear(one_stat_t*s);

#ifdef __cplusplus
}
#endif
#endif
