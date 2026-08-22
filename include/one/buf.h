#ifndef ONE_BUF_H
#define ONE_BUF_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_buf_op {
    ONEBUF_INIT = 0,
    ONEBUF_BEGIN_WRITE,
    ONEBUF_PUBLISH,
    ONEBUF_BEGIN_READ,
    ONEBUF_RELEASE,
    ONEBUF_CHECK,
    ONEBUF_RESET
} one_buf_op_t;

typedef enum one_buf_status {
    ONEBUF_OK = 0,
    ONEBUF_INVALID_ARGUMENT,
    ONEBUF_INVALID_STATE,
    ONEBUF_OVERFLOW,
    ONEBUF_STALE
} one_buf_status_t;

typedef struct one_buf {
    size_t capacity;
    size_t used;
    uint32_t generation;
    uint8_t state;
} one_buf_t;

typedef struct one_buf_result {
    one_buf_status_t status;
    size_t used;
    uint32_t generation;
} one_buf_result_t;

one_buf_result_t one_buf(one_buf_t *buf, one_buf_op_t op, size_t value);

#ifdef __cplusplus
}
#endif

#endif
