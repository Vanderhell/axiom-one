#include "one/dbuf.h"

#define ONE_DBUF_NONE UINT8_C(2)

static one_dbuf_result_t one_dbuf_result(const one_dbuf_t *db, one_dbuf_status_t status,
                                         uint8_t index, uint8_t dropped)
{
    one_dbuf_result_t result;
    result.status = status;
    result.index = index;
    result.used = index < 2u ? db->used[index] : 0u;
    result.generation = index < 2u ? db->generation[index] : 0u;
    result.dropped = dropped;
    return result;
}

static uint8_t one_dbuf_oldest_ready(const one_dbuf_t *db)
{
    if (db->state[0] != ONEDBUF_READY) return db->state[1] == ONEDBUF_READY ? 1u : ONE_DBUF_NONE;
    if (db->state[1] != ONEDBUF_READY) return 0u;
    return (uint32_t)(db->generation[1] - db->generation[0]) < UINT32_C(0x80000000) ? 0u : 1u;
}

one_dbuf_result_t one_dbuf(one_dbuf_t *db, one_dbuf_op_t op, size_t value)
{
    uint8_t index;
    uint8_t other;
    if (db == 0) {
        one_dbuf_result_t result = { ONEDBUF_INVALID_ARGUMENT, ONE_DBUF_NONE, 0u, 0u, 0u };
        return result;
    }

    if (op == ONEDBUF_INIT) {
        if (db->policy != ONEDBUF_REJECT_NEW && db->policy != ONEDBUF_DROP_OLD)
            return one_dbuf_result(db, ONEDBUF_INVALID_ARGUMENT, ONE_DBUF_NONE, 0u);
        db->capacity = value;
        db->used[0] = db->used[1] = 0u;
        db->generation[0] = db->generation[1] = 0u;
        db->next_generation = 0u;
        db->state[0] = db->state[1] = ONEDBUF_FREE;
        db->writer_index = db->reader_index = ONE_DBUF_NONE;
        return one_dbuf_result(db, ONEDBUF_OK, ONE_DBUF_NONE, 0u);
    }

    switch (op) {
    case ONEDBUF_BEGIN_WRITE:
        if (db->writer_index != ONE_DBUF_NONE) return one_dbuf_result(db, ONEDBUF_INVALID_STATE, ONE_DBUF_NONE, 0u);
        index = db->state[0] == ONEDBUF_FREE ? 0u : (db->state[1] == ONEDBUF_FREE ? 1u : ONE_DBUF_NONE);
        if (index == ONE_DBUF_NONE) {
            if (db->policy != ONEDBUF_DROP_OLD) return one_dbuf_result(db, ONEDBUF_FULL, ONE_DBUF_NONE, 0u);
            index = one_dbuf_oldest_ready(db);
            if (index == ONE_DBUF_NONE) return one_dbuf_result(db, ONEDBUF_FULL, ONE_DBUF_NONE, 0u);
            db->used[index] = 0u;
            db->state[index] = ONEDBUF_WRITING;
            db->writer_index = index;
            return one_dbuf_result(db, ONEDBUF_OK, index, 1u);
        }
        db->state[index] = ONEDBUF_WRITING;
        db->writer_index = index;
        return one_dbuf_result(db, ONEDBUF_OK, index, 0u);

    case ONEDBUF_PUBLISH:
        if (db->writer_index >= 2u) return one_dbuf_result(db, ONEDBUF_INVALID_STATE, ONE_DBUF_NONE, 0u);
        if (value > db->capacity) return one_dbuf_result(db, ONEDBUF_OVERFLOW, db->writer_index, 0u);
        index = db->writer_index;
        db->used[index] = value;
        db->generation[index] = ++db->next_generation;
        db->state[index] = ONEDBUF_READY;
        db->writer_index = ONE_DBUF_NONE;
        return one_dbuf_result(db, ONEDBUF_OK, index, 0u);

    case ONEDBUF_BEGIN_READ:
        if (db->reader_index != ONE_DBUF_NONE) return one_dbuf_result(db, ONEDBUF_INVALID_STATE, ONE_DBUF_NONE, 0u);
        index = one_dbuf_oldest_ready(db);
        if (index == ONE_DBUF_NONE) return one_dbuf_result(db, ONEDBUF_EMPTY, ONE_DBUF_NONE, 0u);
        db->state[index] = ONEDBUF_READING;
        db->reader_index = index;
        return one_dbuf_result(db, ONEDBUF_OK, index, 0u);

    case ONEDBUF_RELEASE:
        if (db->reader_index >= 2u) return one_dbuf_result(db, ONEDBUF_INVALID_STATE, ONE_DBUF_NONE, 0u);
        index = db->reader_index;
        db->used[index] = 0u;
        db->state[index] = ONEDBUF_FREE;
        db->reader_index = ONE_DBUF_NONE;
        return one_dbuf_result(db, ONEDBUF_OK, index, 0u);

    case ONEDBUF_RESET:
        db->used[0] = db->used[1] = 0u;
        db->state[0] = db->state[1] = ONEDBUF_FREE;
        db->writer_index = db->reader_index = ONE_DBUF_NONE;
        ++db->next_generation;
        return one_dbuf_result(db, ONEDBUF_OK, ONE_DBUF_NONE, 0u);

    default:
        other = ONE_DBUF_NONE;
        return one_dbuf_result(db, ONEDBUF_INVALID_ARGUMENT, other, 0u);
    }
}
