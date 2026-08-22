#include "one/buf.h"

enum {
    ONE_BUF_FREE = 0,
    ONE_BUF_WRITING,
    ONE_BUF_READY,
    ONE_BUF_READING
};

static one_buf_result_t one_buf_result(const one_buf_t *buf, one_buf_status_t status)
{
    one_buf_result_t result;
    result.status = status;
    result.used = buf->used;
    result.generation = buf->generation;
    return result;
}

one_buf_result_t one_buf(one_buf_t *buf, one_buf_op_t op, size_t value)
{
    if (buf == 0) {
        one_buf_result_t result = { ONEBUF_INVALID_ARGUMENT, 0u, 0u };
        return result;
    }

    switch (op) {
    case ONEBUF_INIT:
        buf->capacity = value;
        buf->used = 0u;
        buf->generation = 0u;
        buf->state = ONE_BUF_FREE;
        return one_buf_result(buf, ONEBUF_OK);

    case ONEBUF_BEGIN_WRITE:
        if (buf->state != ONE_BUF_FREE) return one_buf_result(buf, ONEBUF_INVALID_STATE);
        buf->state = ONE_BUF_WRITING;
        return one_buf_result(buf, ONEBUF_OK);

    case ONEBUF_PUBLISH:
        if (buf->state != ONE_BUF_WRITING) return one_buf_result(buf, ONEBUF_INVALID_STATE);
        if (value > buf->capacity) return one_buf_result(buf, ONEBUF_OVERFLOW);
        buf->used = value;
        ++buf->generation;
        buf->state = ONE_BUF_READY;
        return one_buf_result(buf, ONEBUF_OK);

    case ONEBUF_BEGIN_READ:
        if (buf->state != ONE_BUF_READY) return one_buf_result(buf, ONEBUF_INVALID_STATE);
        buf->state = ONE_BUF_READING;
        return one_buf_result(buf, ONEBUF_OK);

    case ONEBUF_RELEASE:
        if (buf->state != ONE_BUF_READING) return one_buf_result(buf, ONEBUF_INVALID_STATE);
        buf->used = 0u;
        buf->state = ONE_BUF_FREE;
        return one_buf_result(buf, ONEBUF_OK);

    case ONEBUF_CHECK:
        return one_buf_result(buf, value == (size_t)buf->generation ? ONEBUF_OK : ONEBUF_STALE);

    case ONEBUF_RESET:
        buf->used = 0u;
        ++buf->generation;
        buf->state = ONE_BUF_FREE;
        return one_buf_result(buf, ONEBUF_OK);

    default:
        return one_buf_result(buf, ONEBUF_INVALID_ARGUMENT);
    }
}
