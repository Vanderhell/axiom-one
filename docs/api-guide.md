# Axiom One public API guide

## General contract

Public declarations live in `include/one/*.h`; `include/one.h` includes all of them. The public C API and modules retain the `ONE` name, while the project name is Axiom One. Function names are lowercase `one_*`. All headers can be included from C++ and keep C linkage.

Modules either return a typed status enum, a `bool`, or a value whose API has no error condition. A status other than its documented `*_OK` value means output/state must not be assumed to have changed unless the module contract explicitly says otherwise.

## API map

| Module | Main operations |
| --- | --- |
| `ONEARB` | `one_arb_init`, `one_arb_next` |
| `ONEBACKOFF` | `one_backoff_delay` |
| `ONEBUF` / `ONEDBUF` | One dispatcher function driven by `one_buf_op_t` / `one_dbuf_op_t`; inspect its returned result structure. |
| `ONEDEAD` | `one_dead_u32`, `one_dead_i32` |
| `ONEEMA` | `one_ema_init`, `one_ema_update` |
| `ONEFIELD` | required-byte queries; range/symbol initialization; lookup by ID, key, or index; clear/fill. |
| `ONEHYST` | `one_hyst_u32`, `one_hyst_i32` |
| `ONEIDX` | range/symbol initialization, index lookup, capacity query. |
| `ONEINIT` | `one_init_run` |
| `ONEOUTLIER` | `one_outlier_init`, `one_outlier_accept` |
| `ONEQUANT` | `one_quant_u32`, `one_quant_i32` |
| `ONERATE` | `one_rate_step` |
| `ONESCALE` | signed/unsigned conversion, with checked and clamping variants. |
| `ONESEQ` | initialization, next/previous/advance, distances, comparison, and forward-window membership. |
| `ONESET` | required-byte queries; range/symbol initialization; index/key conversion; add/remove/contains by ID, key, or index; clear/fill. |
| `ONESLEW` | `one_slew_u32`, `one_slew_i32` |
| `ONESTALE` | `one_stale_check` |
| `ONESTAT` | required-byte queries; initialization; get/set/inc/dec/add/reset by ID, key, or index; clear. |
| `ONETRACE` | required-byte query; init, push, clear, count, chronological get. |
| `ONEVOTE` | init, push, get, reset. |
| `ONEWINDOW` | reset and accept. |

## Status and ownership conventions

- Output pointers are required when a function has an output argument. Check the status before reading the output.
- `ONEBUF` and `ONEDBUF` use their operation enum as a small state-machine protocol. A result includes status and the relevant size/generation/index data.
- `ONEFIELD`, `ONESET`, and `ONESTAT` do not allocate storage. Call their required-byte API first when sizing storage dynamically or conservatively.
- `ONEEMA`, `ONEOUTLIER`, `ONEVOTE`, `ONEWINDOW`, and similar modules retain state only in the caller-provided structure.
- Stateless modules (`ONEBACKOFF`, `ONEDEAD`, `ONEHYST`, `ONEQUANT`, `ONESCALE`, `ONESLEW`, `ONESTALE`) do not need initialization.

## Numeric rules worth remembering

- `ONESLEW` treats zero rise/fall limits as valid holds and never overshoots the target.
- `ONEHYST` requires `low < high`; its state must be 0 or 1.
- `ONESCALE` rounds to the nearest representable output. Non-clamping calls reject out-of-range inputs.
- `ONEQUANT` chooses the nearest grid point and handles signed domains without signed-overflow shortcuts.
- `ONESTALE` and `ONEWINDOW` use half-range (`0x80000000`) ambiguity rules for wrap-safe 32-bit arithmetic.
- `ONESEQ` makes ambiguity explicit when a cyclic distance is exactly half the configured capacity.

For exact signatures, enum members, limits, and error codes, include the corresponding public module header.
