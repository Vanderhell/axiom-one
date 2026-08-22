#ifndef ONE_INIT_H
#define ONE_INIT_H

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*one_init_fn_t)(void);
#define ONE_COUNT(array) (sizeof(array) / sizeof((array)[0]))
void one_init_run(const one_init_fn_t *items, size_t count);
#ifdef __cplusplus
}
#endif
#endif
