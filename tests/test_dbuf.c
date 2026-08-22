#include "test_assert.h"
#include <stdint.h>

#include "one/dbuf.h"

int main(void)
{
    one_dbuf_t db = { 0 };
    one_dbuf_result_t result;
    uint8_t first;
    uint8_t second;

    assert(one_dbuf(0, ONEDBUF_INIT, 4u).status == ONEDBUF_INVALID_ARGUMENT);
    db.policy = (one_dbuf_policy_t)99;
    assert(one_dbuf(&db, ONEDBUF_INIT, 4u).status == ONEDBUF_INVALID_ARGUMENT);
    db.policy = ONEDBUF_REJECT_NEW;

    result = one_dbuf(&db, ONEDBUF_INIT, 4u);
    assert(result.status == ONEDBUF_OK);
    assert(db.state[0] == ONEDBUF_FREE && db.state[1] == ONEDBUF_FREE);
    assert(one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u).status == ONEDBUF_EMPTY);
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 0u).status == ONEDBUF_INVALID_STATE);
    result = one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u);
    assert(result.status == ONEDBUF_OK);
    first = result.index;
    assert(one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).status == ONEDBUF_INVALID_STATE);
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 5u).status == ONEDBUF_OVERFLOW);
    result = one_dbuf(&db, ONEDBUF_PUBLISH, 4u);
    assert(result.status == ONEDBUF_OK && result.index == first && result.used == 4u && result.generation == 1u);
    result = one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u);
    assert(result.status == ONEDBUF_OK && result.index == first && result.used == 4u);
    assert(one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u).status == ONEDBUF_INVALID_STATE);
    result = one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u);
    assert(result.status == ONEDBUF_OK && result.index != first);
    second = result.index;
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 2u).status == ONEDBUF_OK);
    assert(one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).status == ONEDBUF_FULL);
    assert(one_dbuf(&db, ONEDBUF_RELEASE, 0u).status == ONEDBUF_OK);
    result = one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u);
    assert(result.status == ONEDBUF_OK && result.index == second && result.generation == 2u);
    assert(one_dbuf(&db, ONEDBUF_RELEASE, 0u).status == ONEDBUF_OK);

    db.policy = ONEDBUF_DROP_OLD;
    assert(one_dbuf(&db, ONEDBUF_INIT, 4u).status == ONEDBUF_OK);
    first = one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).index;
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 1u).status == ONEDBUF_OK);
    assert(one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u).index == first);
    second = one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).index;
    assert(second != first && one_dbuf(&db, ONEDBUF_PUBLISH, 1u).status == ONEDBUF_OK);
    result = one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u);
    assert(result.status == ONEDBUF_OK && result.index == second && result.dropped == 1u);
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 3u).status == ONEDBUF_OK);
    assert(one_dbuf(&db, ONEDBUF_RELEASE, 0u).status == ONEDBUF_OK);
    result = one_dbuf(&db, ONEDBUF_BEGIN_READ, 0u);
    assert(result.status == ONEDBUF_OK && result.index == second && result.used == 3u && result.generation == 3u);
    assert(one_dbuf(&db, ONEDBUF_RELEASE, 0u).status == ONEDBUF_OK);

    db.next_generation = UINT32_MAX;
    assert(one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).status == ONEDBUF_OK);
    assert(one_dbuf(&db, ONEDBUF_PUBLISH, 0u).generation == 0u);
    assert(one_dbuf(&db, ONEDBUF_RESET, 0u).status == ONEDBUF_OK);
    assert(db.state[0] == ONEDBUF_FREE && db.state[1] == ONEDBUF_FREE);
    assert(one_dbuf(&db, ONEDBUF_RELEASE, 0u).status == ONEDBUF_INVALID_STATE);
    assert(one_dbuf(&db, (one_dbuf_op_t)99, 0u).status == ONEDBUF_INVALID_ARGUMENT);
    return 0;
}
