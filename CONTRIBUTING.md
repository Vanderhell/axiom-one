# Contributing to Axiom One

Thank you for improving Axiom One. Small, reviewable changes are preferred because every module has a deliberately narrow contract.

## Before opening an issue

- Search existing issues and public documentation.
- Reduce a bug to the smallest C99 reproducer.
- State the compiler, version, target platform, build configuration, and observed/expected behavior.

## Development workflow

1. Create a focused branch from the current default branch.
2. Keep the module boundary intact: do not combine unrelated policies in one primitive.
3. Add or update tests with every behavioral change. Cover success, boundary, invalid-argument, and state-preservation paths where applicable.
4. Update the relevant quick start or API guide when public behavior changes.
5. Run the validation matrix before requesting review.

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure

cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Also run ASan and the warning build when they are available; see [docs/validation.md](docs/validation.md).

## Code style

- C99 only; preserve C++ `extern "C"` header compatibility.
- Use `one_*` for public names and fixed-width integer types where the API already uses them.
- Do not add allocation, I/O, callbacks, clocks, global mutable state, or compiler-specific extensions without an explicit module-contract discussion.
- Keep public headers self-contained and update umbrella-header coverage when adding a module.

## Pull requests

Describe the problem, contract impact, implementation, tests, and documentation changes. Keep generated build output out of commits. By contributing, you agree that your contribution is licensed under the repository's [MIT License](LICENSE).
