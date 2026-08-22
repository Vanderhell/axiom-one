#include "test_assert.h"

#include "one/set.h"

int main(void)
{
    uint8_t storage[40] = { 0 };
    one_set_t set;
    size_t bytes;
    size_t index;
    bool contains;
    static const one_set_position_t schema[] = {
        ONESET_CHARS("ABC"), ONESET_CHARS("01"), ONESET_CHARS("xy")
    };
    static const one_set_position_t duplicate[] = { ONESET_CHARS("ABCA") };
    const char *first = "A0x";
    const char *last = "C1y";
    size_t i;

    assert(one_set_required_bytes_range(1u, 500u, &bytes) == ONESET_OK && bytes == 63u);
    assert(one_set_required_bytes_range(0u, 256u, &bytes) == ONESET_OK && bytes == 33u);
    assert(one_set_init_range(&set, 100u, 199u, storage, sizeof(storage)) == ONESET_OK);
    one_set_clear(&set);
    assert(one_set_add_u32(&set, 100u) == ONESET_OK);
    assert(one_set_add_u32(&set, 199u) == ONESET_OK);
    assert(one_set_contains_u32(&set, 100u) && one_set_contains_u32(&set, 199u));
    assert(!one_set_contains_u32(&set, 99u));
    assert(one_set_index_u32(&set, 199u, &index) == ONESET_OK && index == 99u);
    assert(one_set_remove_u32(&set, 100u) == ONESET_OK && !one_set_contains_u32(&set, 100u));
    one_set_fill(&set);
    for (i = 100u; i <= 199u; ++i) assert(one_set_contains_u32(&set, (uint32_t)i));
    one_set_clear(&set);
    for (i = 100u; i <= 199u; ++i) assert(!one_set_contains_u32(&set, (uint32_t)i));
    assert(one_set_contains_u32_checked(&set, 99u, &contains) == ONESET_ERR_INVALID_KEY);
    assert(one_set_contains_u32_checked(&set, 101u, 0) == ONESET_ERR_INVALID_ARGUMENT);
    one_set_add_index(&set, 1u);
    assert(one_set_contains_index(&set, 1u));
    one_set_remove_index(&set, 1u);
    assert(!one_set_contains_index(&set, 1u));

    assert(one_set_init_symbols(&set, duplicate, 1u, storage, sizeof(storage)) == ONESET_ERR_DUPLICATE_SYMBOL);
    assert(one_set_required_bytes_symbols(schema, 3u, &bytes) == ONESET_OK && bytes == 2u);
    assert(one_set_init_symbols(&set, schema, 3u, storage, sizeof(storage)) == ONESET_OK);
    one_set_clear(&set);
    for (i = 0u; i < 12u; ++i) {
        char key[4];
        key[0] = "ABC"[i / 4u];
        key[1] = "01"[(i / 2u) % 2u];
        key[2] = "xy"[i % 2u];
        key[3] = '\0';
        assert(one_set_index(&set, (const uint8_t *)key, &index) == ONESET_OK && index == i);
        assert(one_set_add(&set, (const uint8_t *)key) == ONESET_OK);
    }
    for (i = 0u; i < 12u; ++i) assert(one_set_contains_index(&set, i));
    assert(one_set_remove(&set, (const uint8_t *)first) == ONESET_OK);
    assert(one_set_remove(&set, (const uint8_t *)last) == ONESET_OK);
    assert(!one_set_contains(&set, (const uint8_t *)first));
    assert(!one_set_contains(&set, (const uint8_t *)last));
    one_set_fill(&set);
    for (i = 0u; i < 12u; ++i) assert(one_set_contains_index(&set, i));
    assert(!one_set_contains_index(&set, 12u));
    assert(!one_set_contains(&set, (const uint8_t *)"C2x"));
    assert(one_set_contains_checked(&set, (const uint8_t *)"B0x", &contains) == ONESET_OK && contains);
    assert(one_set_contains_checked(&set, 0, &contains) == ONESET_ERR_INVALID_ARGUMENT);
    assert(one_set_contains_checked(&set, (const uint8_t *)"B0x", 0) == ONESET_ERR_INVALID_ARGUMENT);
    assert(one_set_add_u32(&set, 1u) == ONESET_ERR_WRONG_MODE);
    assert(one_set_init_range(&set, 2u, 1u, storage, sizeof(storage)) == ONESET_ERR_INVALID_RANGE);
    assert(one_set_init_range(&set, 0u, 7u, storage, 0u) == ONESET_ERR_STORAGE_TOO_SMALL);
    assert(one_set_required_bytes_range(2u, 1u, &bytes) == ONESET_ERR_INVALID_RANGE);
    assert(one_set_required_bytes_range(0u, 7u, 0) == ONESET_ERR_INVALID_ARGUMENT);
    return 0;
}
