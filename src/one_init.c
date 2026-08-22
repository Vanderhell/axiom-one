#include "one/init.h"

void one_init_run(const one_init_fn_t *items, size_t count)
{
    size_t index;
    if (items == 0) { return; }
    for (index = 0u; index < count; ++index) {
        if (items[index] != 0) { items[index](); }
    }
}
