#ifndef ONE_DBUF_H
#define ONE_DBUF_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum one_dbuf_op {
    ONEDBUF_INIT = 0,
    ONEDBUF_BEGIN_WRITE,
    ONEDBUF_PUBLISH,
    ONEDBUF_BEGIN_READ,
    ONEDBUF_RELEASE,
    ONEDBUF_RESET
} one_dbuf_op_t;

typedef enum one_dbuf_state {
    ONEDBUF_FREE = 0,
    ONEDBUF_WRITING,
    ONEDBUF_READY,
    ONEDBUF_READING
} one_dbuf_state_t;

typedef enum one_dbuf_policy {
    ONEDBUF_REJECT_NEW = 0,
    ONEDBUF_DROP_OLD
} one_dbuf_policy_t;

typedef enum one_dbuf_status {
    ONEDBUF_OK = 0,
    ONEDBUF_EMPTY,
    ONEDBUF_FULL,
    ONEDBUF_INVALID_ARGUMENT,
    ONEDBUF_INVALID_STATE,
    ONEDBUF_OVERFLOW
} one_dbuf_status_t;

typedef struct one_dbuf {
    size_t capacity;
    size_t used[2];
    uint32_t generation[2];
    uint32_t next_generation;
    uint8_t state[2];
    uint8_t writer_index;
    uint8_t reader_index;
    uint8_t policy;
} one_dbuf_t;

typedef struct one_dbuf_result {
    one_dbuf_status_t status;
    uint8_t index;
    size_t used;
    uint32_t generation;
    uint8_t dropped;
} one_dbuf_result_t;

one_dbuf_result_t one_dbuf(one_dbuf_t *db, one_dbuf_op_t op, size_t value);

#ifdef __cplusplus
}
#endif

#endif
