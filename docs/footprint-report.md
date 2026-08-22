# ONE static-link footprint report

Configuration: Release

Every consumer includes `one.h`; the baseline calls no ONE API. `Delta` therefore measures the incremental PE footprint relative to inclusion of the umbrella header alone.

| Target | EXE bytes | Delta EXE | .text raw | Delta .text | Linked ONE objects |
| --- | ---: | ---: | ---: | ---: | --- |
| one_footprint_baseline | 10240 | 0 | 3584 | 0 |  |
| one_footprint_arb | 10240 | 0 | 3584 | 0 | one_arb.obj |
| one_footprint_backoff | 10240 | 0 | 3584 | 0 | one_backoff.obj |
| one_footprint_buf | 11776 | 1536 | 4608 | 1024 | one_buf.obj |
| one_footprint_combo_all | 17408 | 7168 | 9216 | 5632 | one_arb.obj, one_backoff.obj, one_buf.obj, one_dbuf.obj, one_dead.obj, one_ema.obj, one_field.obj, one_hyst.obj, one_idx.obj, one_init.obj, one_outlier.obj, one_quant.obj, one_rate.obj, one_scale.obj, one_seq.obj, one_set.obj, one_slew.obj, one_stale.obj, one_stat.obj, one_trace.obj, one_vote.obj, one_window.obj |
| one_footprint_combo_numeric | 10752 | 512 | 4096 | 512 | one_dead.obj, one_hyst.obj, one_quant.obj, one_rate.obj, one_scale.obj, one_slew.obj |
| one_footprint_combo_protocol | 11776 | 1536 | 4608 | 1024 | one_init.obj, one_seq.obj, one_window.obj |
| one_footprint_combo_signal | 11776 | 1536 | 4608 | 1024 | one_ema.obj, one_outlier.obj, one_stale.obj, one_vote.obj |
| one_footprint_combo_storage | 14848 | 4608 | 7168 | 3584 | one_buf.obj, one_dbuf.obj, one_field.obj, one_idx.obj, one_set.obj, one_stat.obj, one_trace.obj |
| one_footprint_dbuf | 12288 | 2048 | 5120 | 1536 | one_dbuf.obj |
| one_footprint_dead | 10240 | 0 | 3584 | 0 | one_dead.obj |
| one_footprint_ema | 11264 | 1024 | 4096 | 512 | one_ema.obj |
| one_footprint_field | 10240 | 0 | 3584 | 0 | one_field.obj |
| one_footprint_hyst | 10240 | 0 | 3584 | 0 | one_hyst.obj |
| one_footprint_idx | 10240 | 0 | 3584 | 0 | one_idx.obj |
| one_footprint_init | 10240 | 0 | 3584 | 0 | one_init.obj |
| one_footprint_outlier | 10240 | 0 | 3584 | 0 | one_outlier.obj |
| one_footprint_quant | 10240 | 0 | 3584 | 0 | one_quant.obj |
| one_footprint_rate | 10240 | 0 | 3584 | 0 | one_rate.obj |
| one_footprint_scale | 10240 | 0 | 3584 | 0 | one_scale.obj |
| one_footprint_seq | 11264 | 1024 | 4096 | 512 | one_seq.obj |
| one_footprint_set | 10240 | 0 | 3584 | 0 | one_set.obj |
| one_footprint_slew | 10240 | 0 | 3584 | 0 | one_slew.obj |
| one_footprint_stale | 10240 | 0 | 3584 | 0 | one_stale.obj |
| one_footprint_stat | 10752 | 512 | 4096 | 512 | one_stat.obj |
| one_footprint_trace | 10240 | 0 | 3584 | 0 | one_trace.obj |
| one_footprint_vote | 10240 | 0 | 3584 | 0 | one_vote.obj |
| one_footprint_window | 10240 | 0 | 3584 | 0 | one_window.obj |
