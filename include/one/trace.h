#ifndef ONE_TRACE_H
#define ONE_TRACE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t one_trace_step_t;
typedef enum one_trace_status { ONETRACE_OK = 0, ONETRACE_ERR_INVALID_ARGUMENT, ONETRACE_ERR_INVALID_CAPACITY, ONETRACE_ERR_INVALID_STEP_WIDTH, ONETRACE_ERR_STEP_OUT_OF_RANGE, ONETRACE_ERR_INDEX_OUT_OF_RANGE, ONETRACE_ERR_OVERFLOW } one_trace_status_t;
typedef struct one_trace { uint8_t *buffer; size_t buffer_size; size_t capacity; size_t head; size_t count; uint8_t step_bits; } one_trace_t;
one_trace_status_t one_trace_required_bytes(size_t capacity,uint8_t step_bits,size_t*out_bytes);
one_trace_status_t one_trace_init(one_trace_t *trace,void *buffer,size_t buffer_size,size_t capacity,uint8_t step_bits);
one_trace_status_t one_trace_push(one_trace_t *trace,uint32_t step);
void one_trace_clear(one_trace_t *trace);
size_t one_trace_count(const one_trace_t *trace);
one_trace_status_t one_trace_get(const one_trace_t *trace,size_t index,uint32_t*out_step);
#ifdef __cplusplus
}
#endif
#endif
