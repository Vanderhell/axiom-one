# Axiom One validation guide

## Test philosophy

The suite tests all public `one_*` functions directly. It combines API/error tests, state-machine transitions, boundary values, and independent reference models for arithmetic or sliding-state modules.

Notable exhaustive/property coverage includes:

- `ONEARB`: every cursor with request masks `1..65535` plus wrap behavior.
- `ONEBACKOFF`, `ONERATE`, `ONEDEAD`, `ONEHYST`, `ONESLEW`, `ONEOUTLIER`, and `ONEVOTE`: broad combinatorial or reference-model checks.
- `ONESEQ`, `ONESTALE`, and `ONEWINDOW`: wrap-around and half-range ambiguity cases.
- Packed containers: width, storage, symbolic schema, and public error-path checks.

## Required local validation

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure

cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure

cmake --build build-asan --config Debug
ctest --test-dir build-asan -C Debug --output-on-failure
```

For MSVC warning validation, use the existing build directory configured with `/W4 /WX`:

```powershell
cmake --build build-warnings --config Debug
ctest --test-dir build-warnings -C Debug --output-on-failure
```

AddressSanitizer link messages about `/INCREMENTAL` are MSVC linker notices caused by ASan metadata; they are not compiler warnings from ONE source.

## Footprint validation

See the [footprint report](footprint-report.md) and the benchmark commands in the main README. The benchmark uses `one.h` in every consumer and linker maps to show which static-library members are actually selected.
