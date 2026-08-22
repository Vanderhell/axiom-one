#ifndef ONE_WINDOW_H
#define ONE_WINDOW_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { uint32_t newest; uint32_t seen; } one_window_t;
typedef enum { ONE_WINDOW_NEW = 0, ONE_WINDOW_DUPLICATE, ONE_WINDOW_TOO_OLD, ONE_WINDOW_AMBIGUOUS } one_window_result_t;
void one_window_reset(one_window_t *window);
one_window_result_t one_window_accept(one_window_t *window, uint32_t sequence);
#ifdef __cplusplus
}
#endif
#endif
