# ONE module quick starts

Every example is C99 and assumes `#include <one.h>`. State objects belong to the caller and should normally have automatic or application-owned storage.

## ONEARB — round-robin arbiter

```c
one_arb_t arb; uint8_t winner;
one_arb_init(&arb, 0u);
if (one_arb_next(&arb, UINT32_C(0x0000000a), &winner)) { /* winner is 1 or 3 */ }
```

## ONEBACKOFF — saturating exponential delay

```c
uint32_t delay_ms = one_backoff_delay(10u, attempt, 1000u); /* 10, 20, 40 … max 1000 */
```

## ONEBUF — single producer/consumer state buffer

```c
one_buf_t buf;
(void)one_buf(&buf, ONEBUF_INIT, 128u);
if (one_buf(&buf, ONEBUF_BEGIN_WRITE, 0u).status == ONEBUF_OK)
    (void)one_buf(&buf, ONEBUF_PUBLISH, 42u);
if (one_buf(&buf, ONEBUF_BEGIN_READ, 0u).status == ONEBUF_OK)
    (void)one_buf(&buf, ONEBUF_RELEASE, 0u);
```

## ONEDBUF — two-slot producer/consumer buffer

```c
one_dbuf_t db = { 0 };
(void)one_dbuf(&db, ONEDBUF_INIT, 128u);
if (one_dbuf(&db, ONEDBUF_BEGIN_WRITE, 0u).status == ONEDBUF_OK)
    (void)one_dbuf(&db, ONEDBUF_PUBLISH, 42u);
```

Set `db.policy = ONEDBUF_DROP_OLD` before `ONEDBUF_INIT` when replacing the oldest ready item is desired.

## ONEDEAD — dead zone

```c
int32_t output;
if (one_dead_i32(sample, -5, 5, &output) == ONE_DEAD_OK) { /* output is 0 inside [-5, 5] */ }
```

## ONEEMA — integer exponential moving average

```c
one_ema_t ema;
one_ema_init(&ema, 0, 3u);             /* smoothing shift: 1/8 */
int32_t filtered = one_ema_update(&ema, sample);
```

## ONEFIELD — packed fixed-width values

```c
uint8_t storage[8] = { 0 }; one_field_t field; uint32_t value;
(void)one_field_init_range(&field, 100u, 115u, ONEFIELD_BITS_4, storage, sizeof(storage));
(void)one_field_set_u32(&field, 104u, 12u);
(void)one_field_get_u32(&field, 104u, &value);
```

## ONEHYST — two-threshold hysteresis

```c
uint8_t state = 0u;
(void)one_hyst_i32(sample, low_threshold, high_threshold, &state);
```

`state` changes to 0 at/below `low_threshold`, to 1 at/above `high_threshold`, and holds between them.

## ONEIDX — numeric or symbolic key to index

```c
one_idx_t index; size_t slot;
(void)one_idx_init_range(&index, 100u, 199u);
(void)one_idx_index_u32(&index, 123u, &slot); /* slot == 23 */
```

## ONEINIT — ordered initialization

```c
static void driver_init(void) { }
static void app_init(void) { }
const one_init_fn_t init_steps[] = { driver_init, app_init };
one_init_run(init_steps, ONE_COUNT(init_steps));
```

## ONEOUTLIER — accepted-sample delta guard

```c
one_outlier_t guard;
one_outlier_init(&guard, initial_sample, 10u);
if (one_outlier_accept(&guard, sample)) { /* sample became the new accepted reference */ }
```

## ONEQUANT — nearest grid point

```c
int32_t snapped;
(void)one_quant_i32(sensor_value, 0, 25u, &snapped); /* nearest multiple of 25 */
```

## ONERATE — fractional event rate

```c
uint32_t phase = 0u; uint8_t emit;
(void)one_rate_step(3u, 10u, &phase, &emit);
if (emit != 0u) { /* emit on 3 of every 10 calls */ }
```

## ONESCALE — integer range conversion

```c
uint32_t pwm;
if (one_scale_u32(adc, 0u, 4095u, 0u, 1000u, &pwm) == ONESCALE_OK) { }
```

Use `one_scale_*_clamp` when inputs outside the input range must map to an endpoint instead of returning `ONESCALE_ERR_OUT_OF_RANGE`.

## ONESEQ — cyclic sequence domain

```c
one_seq_t sequence; uint32_t next;
(void)one_seq_init_bits(&sequence, 8u);       /* domain 0..255 */
(void)one_seq_next(&sequence, 255u, &next);   /* next == 0 */
```

## ONESET — packed membership set

```c
uint8_t storage[16] = { 0 }; one_set_t set;
(void)one_set_init_range(&set, 0u, 63u, storage, sizeof(storage));
(void)one_set_add_u32(&set, 17u);
if (one_set_contains_u32(&set, 17u)) { }
```

## ONESLEW — bounded step toward target

```c
int32_t command = 0;
(void)one_slew_i32(requested, 5u, 20u, &command); /* rise ≤5, fall ≤20 per call */
```

## ONESTALE — wrap-safe timestamp freshness

```c
if (one_stale_check(now_tick, updated_tick, max_age_ticks) == ONE_STALE_FRESH) { }
```

`max_age_ticks` must be below `0x80000000`; larger values are intentionally ambiguous.

## ONESTAT — packed multi-field records

```c
uint8_t bits[] = { 4u, 8u }; uint8_t storage[16] = { 0 }; one_stat_t stats;
(void)one_stat_init_range(&stats, 0u, 3u, bits, 2u, storage, sizeof(storage));
(void)one_stat_add_u32(&stats, 2u, 0u, 1u); /* saturating add to field 0 of record 2 */
```

## ONETRACE — fixed-capacity ring trace

```c
uint8_t storage[32] = { 0 }; one_trace_t trace;
(void)one_trace_init(&trace, storage, sizeof(storage), 16u, 8u);
(void)one_trace_push(&trace, event_code);
```

When full, the oldest entry is overwritten. Retrieve chronological values with `one_trace_get`.

## ONEVOTE — K-of-N sliding vote

```c
one_vote_t vote;
(void)one_vote_init(&vote, 5u, 3u);
if (one_vote_push(&vote, sensor_is_valid)) { /* at least 3 of last 5 are true */ }
```

## ONEWINDOW — 32-packet replay window

```c
one_window_t window;
one_window_reset(&window);
if (one_window_accept(&window, packet_sequence) == ONE_WINDOW_NEW) { /* accept packet */ }
```

Duplicates, packets older than 31 positions, and half-range ambiguous sequence numbers are classified explicitly.
