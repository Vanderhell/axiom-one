#include "test_assert.h"
#include <stdint.h>

#include "one/field.h"

static void test_width(uint8_t bits, uint32_t maximum)
{
    uint8_t storage[128] = { 0 };
    one_field_t field;
    uint32_t value;
    assert(one_field_init_range(&field, 100u, 120u, bits, storage, sizeof(storage)) == ONEFIELD_OK);
    assert(one_field_set_u32(&field, 110u, maximum) == ONEFIELD_OK);
    assert(one_field_get_u32(&field, 110u, &value) == ONEFIELD_OK && value == maximum);
    if (bits < 32u) assert(one_field_set_u32(&field, 110u, maximum + 1u) == ONEFIELD_ERR_VALUE_OUT_OF_RANGE);
}

int main(void)
{
    uint8_t packed[3] = { 0xFFu, 0xFFu, 0xFFu };
    uint8_t storage[16] = { 0 };
    one_field_t field;
    one_field_t symbols;
    uint32_t value;
    size_t bytes;
    static const one_field_position_t schema[] = {
        ONEFIELD_CHARS("ABC"), ONEFIELD_CHARS("012")
    };
    static const one_field_position_t duplicate[] = { ONEFIELD_CHARS("ABCA") };
    const uint8_t key[] = { 'C', '2' };

    assert(one_field_required_bytes_range(1u, 500u, 2u, &bytes) == ONEFIELD_OK && bytes == 125u);
    assert(one_field_required_bytes_symbols(schema, 2u, 4u, &bytes) == ONEFIELD_OK && bytes == 5u);
    assert(one_field_required_bytes_symbols(0, 0u, 4u, &bytes) == ONEFIELD_ERR_INVALID_ARGUMENT);
    assert(one_field_required_bytes_range(0u, 1u, 1u, 0) == ONEFIELD_ERR_INVALID_ARGUMENT);
    assert(one_field_required_bytes_range(1u, 0u, 2u, &bytes) == ONEFIELD_ERR_INVALID_RANGE);
    assert(one_field_required_bytes_range(0u, 1u, 3u, &bytes) == ONEFIELD_ERR_INVALID_WIDTH);
    assert(one_field_init_range(&field, 0u, 15u, 2u, packed, 2u) == ONEFIELD_ERR_STORAGE_TOO_SMALL);
    assert(one_field_init_range(&field, 0u, 11u, 2u, packed, sizeof(packed)) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 0u, 0u) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 1u, 1u) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 2u, 2u) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 3u, 3u) == ONEFIELD_OK);
    assert(packed[0] == 0xE4u);
    assert(one_field_set_index(&field, 2u, 1u) == ONEFIELD_OK && packed[0] == 0xD4u);
    assert(one_field_get_index(&field, 1u, &value) == ONEFIELD_OK && value == 1u);
    assert(one_field_get_index(&field, 3u, &value) == ONEFIELD_OK && value == 3u);
    assert(one_field_get_u32(&field, 12u, &value) == ONEFIELD_ERR_INDEX_OUT_OF_RANGE);
    one_field_clear(&field);
    assert(packed[0] == 0u && packed[1] == 0u && packed[2] == 0u);

    test_width(1u, 1u); test_width(2u, 3u); test_width(4u, 15u); test_width(8u, 255u);
    test_width(16u, 65535u); test_width(32u, UINT32_MAX);
    assert(one_field_init_range(&field, 0u, 1u, 16u, storage, sizeof(storage)) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 0u, UINT32_C(0x1234)) == ONEFIELD_OK);
    assert(storage[0] == 0x34u && storage[1] == 0x12u);
    assert(one_field_init_range(&field, 0u, 1u, 32u, storage, sizeof(storage)) == ONEFIELD_OK);
    assert(one_field_set_index(&field, 0u, UINT32_C(0x12345678)) == ONEFIELD_OK);
    assert(storage[0] == 0x78u && storage[1] == 0x56u && storage[2] == 0x34u && storage[3] == 0x12u);

    assert(one_field_init_symbols(&symbols, duplicate, 1u, 2u, storage, sizeof(storage)) == ONEFIELD_ERR_DUPLICATE_SYMBOL);
    assert(one_field_init_symbols(&symbols, schema, 2u, 4u, storage, sizeof(storage)) == ONEFIELD_OK);
    assert(one_field_set(&symbols, key, 15u) == ONEFIELD_OK);
    assert(one_field_get(&symbols, key, &value) == ONEFIELD_OK && value == 15u);
    assert(one_field_get_u32(&symbols, 0u, &value) == ONEFIELD_ERR_WRONG_MODE);
    assert(one_field_set(&symbols, (const uint8_t *)"D0", 1u) == ONEFIELD_ERR_INVALID_KEY);
    assert(one_field_get(&symbols, 0, &value) == ONEFIELD_ERR_INVALID_ARGUMENT);
    assert(one_field_get_index(&field, 0u, 0) == ONEFIELD_ERR_INVALID_ARGUMENT);
    assert(one_field_set_index(0, 0u, 0u) == ONEFIELD_ERR_INVALID_ARGUMENT);
    assert(one_field_fill(&symbols, 3u) == ONEFIELD_OK);
    assert(one_field_get(&symbols, key, &value) == ONEFIELD_OK && value == 3u);
    for (uint8_t bits = 2u; bits <= 4u; bits += 2u) {
        uint8_t exhaustive_storage[8] = { 0 };
        uint32_t maximum = bits == 2u ? 3u : 15u;
        assert(one_field_init_range(&field, 0u, 15u, bits, exhaustive_storage, sizeof(exhaustive_storage)) == ONEFIELD_OK);
        for (size_t index = 0u; index < 16u; ++index) {
            for (size_t reset_index = 0u; reset_index < 16u; ++reset_index)
                assert(one_field_set_index(&field, reset_index, maximum) == ONEFIELD_OK);
            for (uint32_t candidate = 0u; candidate <= maximum; ++candidate) {
                assert(one_field_set_index(&field, index, candidate) == ONEFIELD_OK);
                assert(one_field_get_index(&field, index, &value) == ONEFIELD_OK && value == candidate);
                if (index > 0u) assert(one_field_get_index(&field, index - 1u, &value) == ONEFIELD_OK && value == maximum);
                if (index + 1u < 16u) assert(one_field_get_index(&field, index + 1u, &value) == ONEFIELD_OK && value == maximum);
            }
        }
    }
    return 0;
}
