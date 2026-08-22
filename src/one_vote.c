#include "one/vote.h"

bool one_vote_init(one_vote_t *vote, uint8_t window, uint8_t required)
{
    if ((vote == 0) || (window == 0u) || (window > ONE_VOTE_MAX_WINDOW) || (required == 0u) || (required > window)) { return false; }
    vote->bits = 0u; vote->window = window; vote->required = required; vote->count = 0u;
    return true;
}

bool one_vote_push(one_vote_t *vote, bool value)
{
    uint32_t oldest;
    uint32_t mask;
    if ((vote == 0) || (vote->window == 0u) || (vote->window > ONE_VOTE_MAX_WINDOW) || (vote->required == 0u) || (vote->required > vote->window)) { return false; }
    oldest = (vote->bits >> (vote->window - 1u)) & 1u;
    if (oldest != 0u) { --vote->count; }
    mask = (vote->window == 32u) ? UINT32_MAX : ((UINT32_C(1) << vote->window) - 1u);
    vote->bits = ((vote->bits << 1u) | (value ? 1u : 0u)) & mask;
    if (value) { ++vote->count; }
    return vote->count >= vote->required;
}

bool one_vote_get(const one_vote_t *vote)
{
    return (vote != 0) && (vote->count >= vote->required);
}

void one_vote_reset(one_vote_t *vote)
{
    if (vote != 0) { vote->bits = 0u; vote->count = 0u; }
}
