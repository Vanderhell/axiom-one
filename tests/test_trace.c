#include "test_assert.h"
#include <stdint.h>
#include "one/trace.h"

static void test_width(uint8_t bits, uint32_t maximum)
{
    uint8_t buffer[32] = { 0 };
    one_trace_t trace;
    uint32_t value;
    size_t bytes;

    assert(one_trace_required_bytes(7u, bits, &bytes) == ONETRACE_OK && bytes == 7u * (bits / 8u));
    assert(one_trace_init(&trace, buffer, sizeof(buffer), 3u, bits) == ONETRACE_OK);
    assert(one_trace_push(&trace, maximum) == ONETRACE_OK);
    assert(one_trace_get(&trace, 0u, &value) == ONETRACE_OK && value == maximum);
    if (bits < 32u) assert(one_trace_push(&trace, maximum + 1u) == ONETRACE_ERR_STEP_OUT_OF_RANGE);
}

int main(void)
{
    uint8_t buffer[32] = { 0 };
    one_trace_t trace;
    uint32_t value;
    size_t index;

    test_width(8u, UINT8_MAX); test_width(16u, UINT16_MAX); test_width(32u, UINT32_MAX);
    assert(one_trace_required_bytes(0u, 8u, &index) == ONETRACE_ERR_INVALID_CAPACITY);
    assert(one_trace_required_bytes(1u, 7u, &index) == ONETRACE_ERR_INVALID_STEP_WIDTH);
    assert(one_trace_required_bytes(1u, 8u, 0) == ONETRACE_ERR_INVALID_ARGUMENT);
    assert(one_trace_init(0, buffer, sizeof(buffer), 1u, 8u) == ONETRACE_ERR_INVALID_ARGUMENT);
    assert(one_trace_init(&trace, 0, sizeof(buffer), 1u, 8u) == ONETRACE_ERR_INVALID_ARGUMENT);
    assert(one_trace_init(&trace, buffer, 0u, 1u, 8u) == ONETRACE_ERR_INVALID_ARGUMENT);
    assert(one_trace_init(&trace, buffer, sizeof(buffer), 4u, 8u) == ONETRACE_OK);
    for (index = 0u; index < 10u; ++index) assert(one_trace_push(&trace, (uint32_t)index) == ONETRACE_OK);
    assert(one_trace_count(&trace) == 4u);
    for (index = 0u; index < 4u; ++index) assert(one_trace_get(&trace, index, &value) == ONETRACE_OK && value == index + 6u);
    assert(one_trace_get(&trace, 4u, &value) == ONETRACE_ERR_INDEX_OUT_OF_RANGE);
    assert(one_trace_get(&trace, 0u, 0) == ONETRACE_ERR_INVALID_ARGUMENT);
    assert(one_trace_push(0, 0u) == ONETRACE_ERR_INVALID_ARGUMENT);
    one_trace_clear(&trace);
    assert(one_trace_count(&trace) == 0u && one_trace_count(0) == 0u);
    return 0;
}
