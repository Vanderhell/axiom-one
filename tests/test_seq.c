#include "test_assert.h"
#include <stdint.h>

#include "one/seq.h"

static void expect_distance(const one_seq_t *seq, uint32_t from, uint32_t to, uint64_t expected)
{
    uint64_t distance;
    assert(one_seq_forward_distance(seq, from, to, &distance) == ONE_SEQ_OK);
    assert(distance == expected);
}

int main(void)
{
    one_seq_t seq;
    one_seq_t arbitrary;
    one_seq_t singleton;
    one_seq_order_t order;
    uint32_t value;
    uint64_t distance;
    bool inside;
    uint32_t a;
    uint32_t b;

    assert(one_seq_init_bits(&seq, 0u) == ONE_SEQ_ERR_INVALID_BITS);
    assert(one_seq_init_bits(&seq, 33u) == ONE_SEQ_ERR_INVALID_BITS);
    assert(one_seq_init_bits(0, 8u) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    assert(one_seq_init_range(&seq, 2u, 1u) == ONE_SEQ_ERR_INVALID_RANGE);
    assert(one_seq_init_bits(&seq, 8u) == ONE_SEQ_OK && seq.capacity == 256u);
    assert(one_seq_next(&seq, 255u, &value) == ONE_SEQ_OK && value == 0u);
    assert(one_seq_prev(&seq, 0u, &value) == ONE_SEQ_OK && value == 255u);
    assert(one_seq_advance(&seq, 254u, 5u, &value) == ONE_SEQ_OK && value == 3u);
    assert(one_seq_advance(&seq, 254u, 256u, &value) == ONE_SEQ_OK && value == 254u);
    expect_distance(&seq, 0u, 0u, 0u);
    expect_distance(&seq, 0u, 1u, 1u);
    expect_distance(&seq, 255u, 0u, 1u);
    expect_distance(&seq, 254u, 1u, 3u);
    expect_distance(&seq, 0u, 255u, 255u);
    assert(one_seq_backward_distance(&seq, 1u, 254u, &distance) == ONE_SEQ_OK && distance == 3u);
    assert(one_seq_compare(&seq, 0u, 255u, &order) == ONE_SEQ_OK && order == ONE_SEQ_ORDER_NEWER);
    assert(one_seq_compare(&seq, 255u, 0u, &order) == ONE_SEQ_OK && order == ONE_SEQ_ORDER_OLDER);
    assert(one_seq_compare(&seq, 42u, 42u, &order) == ONE_SEQ_OK && order == ONE_SEQ_ORDER_EQUAL);
    assert(one_seq_compare(&seq, 128u, 0u, &order) == ONE_SEQ_OK && order == ONE_SEQ_ORDER_AMBIGUOUS);
    assert(one_seq_in_forward_window(&seq, 250u, 4u, 10u, &inside) == ONE_SEQ_OK && inside);
    assert(one_seq_in_forward_window(&seq, 250u, 5u, 10u, &inside) == ONE_SEQ_OK && !inside);
    assert(one_seq_in_forward_window(&seq, 250u, 250u, 256u, &inside) == ONE_SEQ_ERR_WINDOW_OUT_OF_RANGE);

    assert(one_seq_init_range(&arbitrary, 100u, 199u) == ONE_SEQ_OK);
    assert(one_seq_next(&arbitrary, 199u, &value) == ONE_SEQ_OK && value == 100u);
    assert(one_seq_prev(&arbitrary, 100u, &value) == ONE_SEQ_OK && value == 199u);
    expect_distance(&arbitrary, 198u, 101u, 3u);
    assert(one_seq_next(&arbitrary, 99u, &value) == ONE_SEQ_ERR_VALUE_OUT_OF_RANGE);
    assert(one_seq_next(&arbitrary, 200u, &value) == ONE_SEQ_ERR_VALUE_OUT_OF_RANGE);

    assert(one_seq_init_range(&singleton, 5u, 5u) == ONE_SEQ_OK);
    assert(one_seq_next(&singleton, 5u, &value) == ONE_SEQ_OK && value == 5u);
    assert(one_seq_compare(&singleton, 5u, 5u, &order) == ONE_SEQ_OK && order == ONE_SEQ_ORDER_EQUAL);
    assert(one_seq_init_bits(&seq, 32u) == ONE_SEQ_OK);
    assert(one_seq_next(&seq, UINT32_MAX, &value) == ONE_SEQ_OK && value == 0u);
    expect_distance(&seq, UINT32_MAX, 1u, 2u);

    assert(one_seq_init_bits(&seq, 8u) == ONE_SEQ_OK);
    assert(one_seq_advance(&seq, 1u, 1u, 0) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    assert(one_seq_prev(&seq, 1u, 0) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    assert(one_seq_forward_distance(&seq, 1u, 2u, 0) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    assert(one_seq_compare(&seq, 1u, 2u, 0) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    assert(one_seq_in_forward_window(&seq, 1u, 2u, 1u, 0) == ONE_SEQ_ERR_INVALID_ARGUMENT);
    for (a = 0u; a < 256u; ++a) {
        for (b = 0u; b < 256u; ++b) {
            uint64_t reverse_distance;
            assert(one_seq_forward_distance(&seq, a, b, &distance) == ONE_SEQ_OK);
            assert(one_seq_forward_distance(&seq, b, a, &reverse_distance) == ONE_SEQ_OK);
            assert((a == b && distance == 0u && reverse_distance == 0u) ||
                   (a != b && distance + reverse_distance == seq.capacity));
            assert(one_seq_compare(&seq, a, b, &order) == ONE_SEQ_OK);
            assert((a == b && order == ONE_SEQ_ORDER_EQUAL) ||
                   (a != b && reverse_distance == 128u && order == ONE_SEQ_ORDER_AMBIGUOUS) ||
                   (reverse_distance < 128u && order == ONE_SEQ_ORDER_NEWER) ||
                   (reverse_distance > 128u && order == ONE_SEQ_ORDER_OLDER));
        }
    }
    return 0;
}
