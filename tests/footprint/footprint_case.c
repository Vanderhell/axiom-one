#include "one.h"

static volatile uint64_t footprint_sink;

static void footprint_callback(void)
{
    ++footprint_sink;
}

int main(void)
{
#if ONE_FOOTPRINT_CASE == 0
    footprint_sink = 0u;
#elif ONE_FOOTPRINT_CASE == 1
    one_arb_t value; uint8_t winner;
    one_arb_init(&value, 0u); footprint_sink = one_arb_next(&value, 1u, &winner);
#elif ONE_FOOTPRINT_CASE == 2
    footprint_sink = one_backoff_delay(1u, 3u, 64u);
#elif ONE_FOOTPRINT_CASE == 3
    one_ema_t value; one_ema_init(&value, 0, 3u); footprint_sink = (uint32_t)one_ema_update(&value, 10);
#elif ONE_FOOTPRINT_CASE == 4
    one_outlier_t value; one_outlier_init(&value, 0, 10u); footprint_sink = one_outlier_accept(&value, 5);
#elif ONE_FOOTPRINT_CASE == 5
    footprint_sink = one_stale_check(10u, 5u, 10u);
#elif ONE_FOOTPRINT_CASE == 6
    one_vote_t value; footprint_sink = one_vote_init(&value, 3u, 2u) && one_vote_push(&value, true);
#elif ONE_FOOTPRINT_CASE == 7
    uint32_t value; footprint_sink = one_dead_u32(5u, 3u, 8u, &value);
#elif ONE_FOOTPRINT_CASE == 8
    uint8_t value = 0u; footprint_sink = one_hyst_u32(10u, 5u, 8u, &value);
#elif ONE_FOOTPRINT_CASE == 9
    uint32_t value = 0u; footprint_sink = one_slew_u32(10u, 2u, 2u, &value);
#elif ONE_FOOTPRINT_CASE == 10
    uint32_t phase = 0u; uint8_t emit = 0u; footprint_sink = one_rate_step(1u, 10u, &phase, &emit);
#elif ONE_FOOTPRINT_CASE == 11
    one_init_fn_t items[] = { footprint_callback }; one_init_run(items, 1u);
#elif ONE_FOOTPRINT_CASE == 12
    one_window_t value; one_window_reset(&value); footprint_sink = one_window_accept(&value, 1u);
#elif ONE_FOOTPRINT_CASE == 13
    one_seq_t value; uint32_t next; one_seq_init_bits(&value, 8u); footprint_sink = one_seq_next(&value, 1u, &next);
#elif ONE_FOOTPRINT_CASE == 14
    one_buf_t value; footprint_sink = one_buf(&value, ONEBUF_INIT, 16u).status;
#elif ONE_FOOTPRINT_CASE == 15
    one_dbuf_t value; footprint_sink = one_dbuf(&value, ONEDBUF_INIT, 16u).status;
#elif ONE_FOOTPRINT_CASE == 16
    one_field_t value; uint8_t storage[1]; footprint_sink = one_field_init_range(&value, 0u, 1u, ONEFIELD_BITS_1, storage, sizeof(storage));
#elif ONE_FOOTPRINT_CASE == 17
    one_idx_t value; footprint_sink = one_idx_init_range(&value, 0u, 1u);
#elif ONE_FOOTPRINT_CASE == 18
    uint32_t value; footprint_sink = one_quant_u32(7u, 0u, 2u, &value);
#elif ONE_FOOTPRINT_CASE == 19
    uint32_t value; footprint_sink = one_scale_u32(5u, 0u, 10u, 0u, 100u, &value);
#elif ONE_FOOTPRINT_CASE == 20
    one_set_t value; uint8_t storage[1]; footprint_sink = one_set_init_range(&value, 0u, 1u, storage, sizeof(storage));
#elif ONE_FOOTPRINT_CASE == 21
    one_stat_t value; uint8_t bits[1] = { 1u }; uint8_t storage[1]; footprint_sink = one_stat_init_range(&value, 0u, 0u, bits, 1u, storage, sizeof(storage));
#elif ONE_FOOTPRINT_CASE == 22
    one_trace_t value; uint8_t storage[1]; footprint_sink = one_trace_init(&value, storage, sizeof(storage), 1u, 1u);
#elif ONE_FOOTPRINT_CASE == 101
    uint32_t value = 0u, phase = 0u; uint8_t state = 0u, emit = 0u;
    footprint_sink = one_dead_u32(5u, 3u, 8u, &value) + one_hyst_u32(5u, 3u, 8u, &state) + one_slew_u32(10u, 2u, 2u, &value) + one_rate_step(1u, 10u, &phase, &emit) + one_quant_u32(7u, 0u, 2u, &value) + one_scale_u32(5u, 0u, 10u, 0u, 100u, &value);
#elif ONE_FOOTPRINT_CASE == 102
    one_ema_t ema; one_outlier_t outlier; one_vote_t vote;
    one_ema_init(&ema, 0, 3u); one_outlier_init(&outlier, 0, 10u); one_vote_init(&vote, 3u, 2u);
    footprint_sink = (uint32_t)one_ema_update(&ema, 10) + one_outlier_accept(&outlier, 5) + one_vote_push(&vote, true) + one_stale_check(10u, 5u, 10u);
#elif ONE_FOOTPRINT_CASE == 103
    one_window_t window; one_seq_t seq; uint32_t next;
    one_init_fn_t items[] = { footprint_callback }; one_init_run(items, 1u); one_window_reset(&window); one_seq_init_bits(&seq, 8u);
    footprint_sink = one_window_accept(&window, 1u) + one_seq_next(&seq, 1u, &next);
#elif ONE_FOOTPRINT_CASE == 104
    one_buf_t buf; one_dbuf_t dbuf; one_field_t field; one_idx_t idx; one_set_t set; one_stat_t stat; one_trace_t trace;
    uint8_t storage[8], bits[1] = { 1u };
    footprint_sink = one_buf(&buf, ONEBUF_INIT, 8u).status + one_dbuf(&dbuf, ONEDBUF_INIT, 8u).status + one_field_init_range(&field, 0u, 1u, ONEFIELD_BITS_1, storage, sizeof(storage)) + one_idx_init_range(&idx, 0u, 1u) + one_set_init_range(&set, 0u, 1u, storage, sizeof(storage)) + one_stat_init_range(&stat, 0u, 0u, bits, 1u, storage, sizeof(storage)) + one_trace_init(&trace, storage, sizeof(storage), 1u, 1u);
#elif ONE_FOOTPRINT_CASE == 105
    one_arb_t arb; one_ema_t ema; one_outlier_t outlier; one_vote_t vote; one_window_t window; one_seq_t seq;
    uint8_t winner, state = 0u, emit = 0u, storage[8], bits[1] = { 1u }; uint32_t value = 0u, phase = 0u, next;
    one_buf_t buf; one_dbuf_t dbuf; one_field_t field; one_idx_t idx; one_set_t set; one_stat_t stat; one_trace_t trace;
    one_init_fn_t items[] = { footprint_callback };
    one_arb_init(&arb, 0u); one_ema_init(&ema, 0, 3u); one_outlier_init(&outlier, 0, 10u); one_vote_init(&vote, 3u, 2u); one_init_run(items, 1u); one_window_reset(&window); one_seq_init_bits(&seq, 8u);
    footprint_sink = one_arb_next(&arb, 1u, &winner) + one_backoff_delay(1u, 3u, 64u) + (uint32_t)one_ema_update(&ema, 10) + one_outlier_accept(&outlier, 5) + one_stale_check(10u, 5u, 10u) + one_vote_push(&vote, true) + one_dead_u32(5u, 3u, 8u, &value) + one_hyst_u32(5u, 3u, 8u, &state) + one_slew_u32(10u, 2u, 2u, &value) + one_rate_step(1u, 10u, &phase, &emit) + one_window_accept(&window, 1u) + one_seq_next(&seq, 1u, &next) + one_buf(&buf, ONEBUF_INIT, 8u).status + one_dbuf(&dbuf, ONEDBUF_INIT, 8u).status + one_field_init_range(&field, 0u, 1u, ONEFIELD_BITS_1, storage, sizeof(storage)) + one_idx_init_range(&idx, 0u, 1u) + one_quant_u32(7u, 0u, 2u, &value) + one_scale_u32(5u, 0u, 10u, 0u, 100u, &value) + one_set_init_range(&set, 0u, 1u, storage, sizeof(storage)) + one_stat_init_range(&stat, 0u, 0u, bits, 1u, storage, sizeof(storage)) + one_trace_init(&trace, storage, sizeof(storage), 1u, 1u);
#else
#error "Unknown ONE_FOOTPRINT_CASE"
#endif
    return (int)(footprint_sink & 1u);
}
