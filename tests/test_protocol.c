#include "test_assert.h"
#include "one/init.h"
#include "one/window.h"

static int order;
static void first(void) { assert(order == 0); order = 1; }
static void second(void) { assert(order == 1); order = 2; }

static void test_window_wrap_boundaries(void)
{
    static const uint32_t seeds[] = {
        0u, 1u, UINT32_C(0x7fffffff), UINT32_C(0x80000000), UINT32_MAX
    };
    size_t index;

    for (index = 0u; index < sizeof(seeds) / sizeof(seeds[0]); ++index) {
        one_window_t window;
        one_window_t before;
        uint32_t seed = seeds[index];

        one_window_reset(&window);
        assert(one_window_accept(&window, seed) == ONE_WINDOW_NEW);
        before = window;
        assert(one_window_accept(&window, seed + UINT32_C(0x80000000)) == ONE_WINDOW_AMBIGUOUS);
        assert(window.newest == before.newest && window.seen == before.seen);

        assert(one_window_accept(&window, seed + UINT32_C(0x7fffffff)) == ONE_WINDOW_NEW);
        assert(window.newest == seed + UINT32_C(0x7fffffff) && window.seen == 1u);

        one_window_reset(&window);
        assert(one_window_accept(&window, seed) == ONE_WINDOW_NEW);
        before = window;
        assert(one_window_accept(&window, seed + UINT32_C(0x80000001)) == ONE_WINDOW_TOO_OLD);
        assert(window.newest == before.newest && window.seen == before.seen);

        assert(one_window_accept(&window, seed + 31u) == ONE_WINDOW_NEW);
        assert(window.seen == (UINT32_C(1) | (UINT32_C(1) << 31u)));
        assert(one_window_accept(&window, seed) == ONE_WINDOW_DUPLICATE);
    }
}

int main(void)
{
    one_window_t window;
    const one_init_fn_t items[] = { first, second };
    one_init_run(items, ONE_COUNT(items));
    assert(order == 2);
    one_init_run(0, 0u);
    one_window_reset(&window);
    assert(one_window_accept(&window, 100u) == ONE_WINDOW_NEW);
    assert(one_window_accept(&window, 103u) == ONE_WINDOW_NEW);
    assert(one_window_accept(&window, 102u) == ONE_WINDOW_NEW);
    assert(one_window_accept(&window, 102u) == ONE_WINDOW_DUPLICATE);
    assert(one_window_accept(&window, 68u) == ONE_WINDOW_TOO_OLD);
    assert(one_window_accept(&window, UINT32_C(0x80000000) + window.newest) == ONE_WINDOW_AMBIGUOUS);
    one_window_reset(0);
    assert(one_window_accept(0, 1u) == ONE_WINDOW_AMBIGUOUS);
    one_window_reset(&window);
    assert(one_window_accept(&window, UINT32_MAX) == ONE_WINDOW_NEW);
    assert(one_window_accept(&window, 0u) == ONE_WINDOW_NEW);
    test_window_wrap_boundaries();
    return 0;
}
