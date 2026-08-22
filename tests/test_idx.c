#include "test_assert.h"
#include <stdint.h>

#include "one/idx.h"

int main(void)
{
    one_idx_t idx;
    size_t index = 0u;
    uint8_t seen[18] = { 0 };
    uint8_t a;
    uint8_t b;
    static const one_idx_position_t schema[] = { ONEIDX_CHARS("ABC"), ONEIDX_CHARS("012"), ONEIDX_CHARS("XY") };
    static const one_idx_position_t duplicate[] = { ONEIDX_CHARS("ABCA") };

    assert(one_idx_init_range(&idx, 1u, 500u) == ONEIDX_OK);
    assert(one_idx_capacity(&idx) == 500u);
    assert(one_idx_index_u32(&idx, 1u, &index) == ONEIDX_OK && index == 0u);
    assert(one_idx_index_u32(&idx, 217u, &index) == ONEIDX_OK && index == 216u);
    assert(one_idx_index_u32(&idx, 500u, &index) == ONEIDX_OK && index == 499u);
    assert(one_idx_index_u32(&idx, 0u, &index) == ONEIDX_ERR_INVALID_KEY);
    assert(one_idx_index_u32(&idx, 501u, &index) == ONEIDX_ERR_INVALID_KEY);
    assert(one_idx_index(&idx, (const uint8_t *)"A0X", &index) == ONEIDX_ERR_WRONG_MODE);
    assert(one_idx_init_range(&idx, UINT32_MAX, UINT32_MAX) == ONEIDX_OK);
    assert(one_idx_index_u32(&idx, UINT32_MAX, &index) == ONEIDX_OK && index == 0u);
    assert(one_idx_init_range(&idx, 2u, 1u) == ONEIDX_ERR_INVALID_RANGE);

    assert(one_idx_init_symbols(&idx, duplicate, 1u) == ONEIDX_ERR_DUPLICATE_SYMBOL);
    assert(one_idx_init_symbols(&idx, schema, 3u) == ONEIDX_OK);
    assert(one_idx_capacity(&idx) == 18u);
    for (a = 0u; a < 3u; ++a) for (b = 0u; b < 3u; ++b) {
        uint8_t key[] = { (uint8_t)"ABC"[a], (uint8_t)"012"[b], 'X' };
        assert(one_idx_index(&idx, key, &index) == ONEIDX_OK);
        assert(index < 18u && seen[index] == 0u);
        seen[index] = 1u;
        key[2] = 'Y';
        assert(one_idx_index(&idx, key, &index) == ONEIDX_OK);
        assert(index < 18u && seen[index] == 0u);
        seen[index] = 1u;
    }
    for (index = 0u; index < 18u; ++index) assert(seen[index] == 1u);
    assert(one_idx_index(&idx, (const uint8_t *)"C2Y", &index) == ONEIDX_OK && index == 17u);
    assert(one_idx_index(&idx, (const uint8_t *)"D0X", &index) == ONEIDX_ERR_INVALID_KEY);
    assert(one_idx_index_u32(&idx, 0u, &index) == ONEIDX_ERR_WRONG_MODE);
    assert(one_idx_init_symbols(&idx, 0, 0u) == ONEIDX_ERR_INVALID_ARGUMENT);
    assert(one_idx_index(0, (const uint8_t *)"A0X", &index) == ONEIDX_ERR_INVALID_ARGUMENT);
    return 0;
}
