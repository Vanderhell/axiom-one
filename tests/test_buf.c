#include "test_assert.h"
#include <stdint.h>

#include "one/buf.h"

static void assert_unchanged(one_buf_t before, one_buf_t after)
{
    assert(before.capacity == after.capacity);
    assert(before.used == after.used);
    assert(before.generation == after.generation);
    assert(before.state == after.state);
}

int main(void)
{
    one_buf_t buf;
    one_buf_t before;
    one_buf_result_t result;

    result = one_buf(0, ONEBUF_INIT, 16u);
    assert(result.status == ONEBUF_INVALID_ARGUMENT && result.used == 0u && result.generation == 0u);
    result = one_buf(&buf, ONEBUF_INIT, 4u);
    assert(result.status == ONEBUF_OK && result.used == 0u && result.generation == 0u);

    before = buf;
    assert(one_buf(&buf, ONEBUF_PUBLISH, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    before = buf;
    assert(one_buf(&buf, ONEBUF_BEGIN_READ, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    before = buf;
    assert(one_buf(&buf, ONEBUF_RELEASE, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);

    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_OK);
    before = buf;
    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_PUBLISH, 5u).status == ONEBUF_OVERFLOW);
    assert_unchanged(before, buf);
    result = one_buf(&buf, ONEBUF_PUBLISH, 4u);
    assert(result.status == ONEBUF_OK && result.used == 4u && result.generation == 1u);

    before = buf;
    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_PUBLISH, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_RELEASE, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_CHECK, 1u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_CHECK, 0u).status == ONEBUF_STALE);

    result = one_buf(&buf, ONEBUF_BEGIN_READ, 0u);
    assert(result.status == ONEBUF_OK && result.used == 4u && result.generation == 1u);
    before = buf;
    assert(one_buf(&buf, ONEBUF_BEGIN_READ, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    assert(one_buf(&buf, ONEBUF_PUBLISH, 0u).status == ONEBUF_INVALID_STATE);
    assert_unchanged(before, buf);
    result = one_buf(&buf, ONEBUF_RELEASE, 0u);
    assert(result.status == ONEBUF_OK && result.used == 0u && result.generation == 1u);
    assert(one_buf(&buf, ONEBUF_RELEASE, 0u).status == ONEBUF_INVALID_STATE);

    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_PUBLISH, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_CHECK, 1u).status == ONEBUF_STALE);
    assert(one_buf(&buf, ONEBUF_RESET, 0u).status == ONEBUF_OK);
    assert(buf.used == 0u && buf.generation == 3u);
    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_OK);

    assert(one_buf(&buf, ONEBUF_INIT, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_PUBLISH, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_BEGIN_READ, 0u).status == ONEBUF_OK);
    assert(one_buf(&buf, ONEBUF_RELEASE, 0u).status == ONEBUF_OK);

    buf.generation = UINT32_MAX;
    assert(one_buf(&buf, ONEBUF_RESET, 0u).generation == 0u);
    before = buf;
    assert(one_buf(&buf, (one_buf_op_t)99, 0u).status == ONEBUF_INVALID_ARGUMENT);
    assert_unchanged(before, buf);
    return 0;
}
