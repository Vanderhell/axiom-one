#ifndef ONE_SEQ_H
#define ONE_SEQ_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_seq_status {
    ONE_SEQ_OK = 0,
    ONE_SEQ_ERR_INVALID_ARGUMENT,
    ONE_SEQ_ERR_INVALID_RANGE,
    ONE_SEQ_ERR_INVALID_BITS,
    ONE_SEQ_ERR_VALUE_OUT_OF_RANGE,
    ONE_SEQ_ERR_WINDOW_OUT_OF_RANGE,
    ONE_SEQ_ERR_OVERFLOW
} one_seq_status_t;

typedef enum one_seq_order {
    ONE_SEQ_ORDER_EQUAL = 0,
    ONE_SEQ_ORDER_NEWER,
    ONE_SEQ_ORDER_OLDER,
    ONE_SEQ_ORDER_AMBIGUOUS
} one_seq_order_t;

typedef struct one_seq {
    uint32_t min_value;
    uint32_t max_value;
    uint64_t capacity;
} one_seq_t;

one_seq_status_t one_seq_init_range(one_seq_t *seq, uint32_t min_value, uint32_t max_value);
one_seq_status_t one_seq_init_bits(one_seq_t *seq, uint8_t bits);
one_seq_status_t one_seq_next(const one_seq_t *seq, uint32_t value, uint32_t *out_value);
one_seq_status_t one_seq_prev(const one_seq_t *seq, uint32_t value, uint32_t *out_value);
one_seq_status_t one_seq_advance(const one_seq_t *seq, uint32_t value, uint64_t steps, uint32_t *out_value);
one_seq_status_t one_seq_forward_distance(const one_seq_t *seq, uint32_t from, uint32_t to, uint64_t *out_distance);
one_seq_status_t one_seq_backward_distance(const one_seq_t *seq, uint32_t from, uint32_t to, uint64_t *out_distance);
one_seq_status_t one_seq_compare(const one_seq_t *seq, uint32_t a, uint32_t b, one_seq_order_t *out_order);
one_seq_status_t one_seq_in_forward_window(const one_seq_t *seq, uint32_t base, uint32_t value, uint64_t window, bool *out_result);

#ifdef __cplusplus
}
#endif

#endif
