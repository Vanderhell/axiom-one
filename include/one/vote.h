#ifndef ONE_VOTE_H
#define ONE_VOTE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ONE_VOTE_MAX_WINDOW 32u
typedef struct { uint32_t bits; uint8_t window; uint8_t required; uint8_t count; } one_vote_t;
bool one_vote_init(one_vote_t *vote, uint8_t window, uint8_t required);
bool one_vote_push(one_vote_t *vote, bool value);
bool one_vote_get(const one_vote_t *vote);
void one_vote_reset(one_vote_t *vote);

#ifdef __cplusplus
}
#endif
#endif
