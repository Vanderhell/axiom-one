#include "test_assert.h"

#include "one/stat.h"

static void test_width(uint8_t width, uint32_t maximum)
{
    uint8_t storage[16] = { 0 };
    one_stat_t stat;
    uint32_t value;

    assert(one_stat_init_range(&stat, 0u, 1u, &width, 1u, storage, sizeof(storage)) == ONESTAT_OK);
    assert(one_stat_set_index(&stat, 0u, 0u, maximum) == ONESTAT_OK);
    assert(one_stat_inc_index(&stat, 0u, 0u) == ONESTAT_OK);
    assert(one_stat_get_index(&stat, 0u, 0u, &value) == ONESTAT_OK && value == maximum);
    assert(one_stat_dec_index(&stat, 0u, 0u) == ONESTAT_OK);
    assert(one_stat_get_index(&stat, 0u, 0u, &value) == ONESTAT_OK && value == maximum - 1u);
    assert(one_stat_add_index(&stat, 0u, 0u, UINT32_MAX) == ONESTAT_OK);
    assert(one_stat_get_index(&stat, 0u, 0u, &value) == ONESTAT_OK && value == maximum);
    assert(one_stat_set_index(&stat, 1u, 0u, 0u) == ONESTAT_OK);
    assert(one_stat_get_index(&stat, 1u, 0u, &value) == ONESTAT_OK && value == 0u);
}

int main(void)
{
    uint8_t bits[] = { 1u, 16u, 4u, 2u, 32u };
    uint8_t storage[64] = { 0 };
    one_stat_t stat;
    uint32_t value;
    size_t i;
    static const one_stat_position_t positions[] = {
        ONESTAT_CHARS("AB"), ONESTAT_CHARS("01")
    };

    assert(one_stat_required_bytes_range(0u, 1u, bits, 5u, &i) == ONESTAT_OK && i == 14u);
    assert(one_stat_required_bytes_symbols(positions, 2u, bits, 5u, &i) == ONESTAT_OK && i == 28u);
    assert(one_stat_required_bytes_range(2u, 1u, bits, 5u, &i) == ONESTAT_ERR_INVALID_RANGE);
    assert(one_stat_required_bytes_range(0u, 1u, 0, 0u, &i) == ONESTAT_ERR_INVALID_ARGUMENT);

    test_width(1u, 1u); test_width(2u, 3u); test_width(4u, 15u);
    test_width(8u, 255u); test_width(16u, 65535u); test_width(32u, UINT32_MAX);
    assert(one_stat_init_range(&stat, 1u, 3u, bits, 5u, storage, sizeof(storage)) == ONESTAT_OK);
    assert(one_stat_set_u32(&stat, 1u, 1u, UINT32_C(0x1234)) == ONESTAT_OK);
    assert(one_stat_set_u32(&stat, 2u, 1u, 77u) == ONESTAT_OK);
    assert(one_stat_set_u32(&stat, 1u, 2u, 15u) == ONESTAT_OK);
    assert(one_stat_get_u32(&stat, 1u, 1u, &value) == ONESTAT_OK && value == UINT32_C(0x1234));
    assert(one_stat_get_u32(&stat, 2u, 1u, &value) == ONESTAT_OK && value == 77u);
    assert(one_stat_inc_u32(&stat, 1u, 2u) == ONESTAT_OK);
    assert(one_stat_get_u32(&stat, 1u, 2u, &value) == ONESTAT_OK && value == 15u);
    assert(one_stat_dec_u32(&stat, 1u, 0u) == ONESTAT_OK);
    assert(one_stat_add_u32(&stat, 3u, 3u, 100u) == ONESTAT_OK);
    assert(one_stat_get_u32(&stat, 3u, 3u, &value) == ONESTAT_OK && value == 3u);
    assert(one_stat_set_u32(&stat, 1u, 2u, 16u) == ONESTAT_ERR_VALUE_OUT_OF_RANGE);
    assert(one_stat_reset_u32(&stat, 1u, 1u) == ONESTAT_OK);
    assert(one_stat_get_u32(&stat, 1u, 1u, &value) == ONESTAT_OK && value == 0u);
    assert(one_stat_reset_index(&stat, 2u, 1u) == ONESTAT_OK);

    assert(one_stat_init_symbols(&stat, positions, 2u, bits, 5u, storage, sizeof(storage)) == ONESTAT_OK);
    one_stat_clear(&stat);
    for (i = 0u; i < 4u; ++i) {
        char key[] = { "AB"[i / 2u], "01"[i % 2u], '\0' };
        assert(one_stat_set(&stat, (const uint8_t *)key, 3u, (uint32_t)i) == ONESTAT_OK);
        assert(one_stat_set(&stat, (const uint8_t *)key, 1u, (uint32_t)(100u + i)) == ONESTAT_OK);
    }
    for (i = 0u; i < 4u; ++i) {
        char key[] = { "AB"[i / 2u], "01"[i % 2u], '\0' };
        assert(one_stat_get(&stat, (const uint8_t *)key, 3u, &value) == ONESTAT_OK && value == i);
        assert(one_stat_get(&stat, (const uint8_t *)key, 1u, &value) == ONESTAT_OK && value == 100u + i);
    }
    assert(one_stat_add(&stat, (const uint8_t *)"B1", 3u, 10u) == ONESTAT_OK);
    assert(one_stat_get(&stat, (const uint8_t *)"B1", 3u, &value) == ONESTAT_OK && value == 3u);
    assert(one_stat_inc(&stat, (const uint8_t *)"B1", 3u) == ONESTAT_OK);
    assert(one_stat_dec(&stat, (const uint8_t *)"B1", 3u) == ONESTAT_OK);
    assert(one_stat_reset(&stat, (const uint8_t *)"B1", 3u) == ONESTAT_OK);
    assert(one_stat_get(&stat, (const uint8_t *)"B1", 3u, &value) == ONESTAT_OK && value == 0u);
    assert(one_stat_get(&stat, (const uint8_t *)"C1", 0u, &value) == ONESTAT_ERR_INVALID_KEY);
    assert(one_stat_get(&stat, (const uint8_t *)"A0", 9u, &value) == ONESTAT_ERR_FIELD_OUT_OF_RANGE);
    assert(one_stat_get(0, (const uint8_t *)"A0", 0u, &value) == ONESTAT_ERR_INVALID_ARGUMENT);
    assert(one_stat_get(&stat, (const uint8_t *)"A0", 0u, 0) == ONESTAT_ERR_INVALID_ARGUMENT);
    return 0;
}
