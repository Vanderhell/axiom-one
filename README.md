# Axiom One

Axiom One is a small, dependency-free C99 static library of focused embedded-friendly primitives. Its public C API and module family use the `ONE` name (`one_*`, `ONEARB`, `ONESLEW`, and so on). Each module solves one bounded problem: it owns no heap, has no hidden global mutable state, and exposes a plain C API.

## Highlights

- 22 independent modules, available through the umbrella header `#include <one.h>`.
- C99 implementation and C++-compatible public headers.
- Caller-owned state; no allocation, timers, callbacks, or I/O in the library.
- Static-library linking: using `one.h` does not pull every module into the final binary. See the [footprint report](docs/footprint-report.md).
- Debug, Release, AddressSanitizer, and MSVC `/W4 /WX` test configurations are maintained.

## Modules

| Area | Modules | Purpose |
| --- | --- | --- |
| Scheduling | `ONEARB`, `ONEBACKOFF`, `ONERATE`, `ONEINIT` | Arbitration, retry delay, fractional rate generation, ordered initialization. |
| Signal and numeric | `ONEEMA`, `ONEOUTLIER`, `ONEDEAD`, `ONEHYST`, `ONESLEW`, `ONEQUANT`, `ONESCALE` | Filtering, validation, thresholding, bounded motion, quantization, and range conversion. |
| Sequence and protocol | `ONESTALE`, `ONEVOTE`, `ONEWINDOW`, `ONESEQ` | Timestamp freshness, sliding votes, replay window, and cyclic sequence numbers. |
| State containers | `ONEBUF`, `ONEDBUF`, `ONEFIELD`, `ONEIDX`, `ONESET`, `ONESTAT`, `ONETRACE` | Single/double buffers, packed values, lookup schemas, sets, records, and ring traces. |

## Quick start

### CMake consumer

```cmake
add_subdirectory(path/to/one)

add_executable(app main.c)
target_link_libraries(app PRIVATE one)
```

```c
#include <one.h>

int main(void)
{
    uint32_t current = 0u;
    (void)one_slew_u32(100u, 10u, 20u, &current);
    return current == 10u ? 0 : 1;
}
```

For a project that uses only a single module, including its narrow header is equally valid:

```c
#include <one/slew.h>
```

`one.h` is a convenience include only. It does not create references to every module. The static linker extracts only the object members required by called functions.

## Build and test

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

The test suite keeps assertions enabled in Release builds. Typical additional configurations are:

```powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure

cmake --build build-asan --config Debug
ctest --test-dir build-asan -C Debug --output-on-failure
```

## Documentation map

- [Module quick starts](docs/quick-starts.md) — one minimal working usage pattern per module.
- [Public API guide](docs/api-guide.md) — contracts, status handling, state ownership, and API map.
- [Validation guide](docs/validation.md) — test strategy and local validation commands.
- `include/one/*.h` — normative public declarations.

## Integration rules

1. Check status values and boolean returns; do not use a numeric output as an error sentinel.
2. Keep mutable state objects (`one_*_t`) owned and synchronized by the caller.
3. Initialise state before its first use unless the API is stateless.
4. Preserve the documented numeric domains: many APIs intentionally use `uint32_t`, `int32_t`, or `size_t` exactly.
5. Treat `NULL` and invalid configuration results as normal recoverable errors where the API exposes a status.

## Footprint benchmark

The optional benchmark builds one consumer per module plus representative combinations:

```powershell
cmake -S . -B build-footprint -D ONE_BUILD_FOOTPRINT_BENCHMARKS=ON -D BUILD_TESTING=OFF
cmake --build build-footprint --config Release
.\eng\measure-footprint.ps1 -BuildDirectory build-footprint -Configuration Release -OutputPath docs/footprint-report.md
```

The report records PE executable and `.text` sizes together with linker-map evidence of the selected `one_*.obj` members.

## License

Copyright © 2026 Vanderhell. Axiom One is distributed under the [MIT License](LICENSE).
