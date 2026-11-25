# Contributing

## Before making a change

- Build with C++17 and SFML 2.6.1.
- Keep gameplay code in the `space_survivor` namespace.
- Keep ownership local. Prefer values, standard containers, and RAII.
- Do not add a dependency without discussing its size, licence, and build cost.

## Code style

- Format C++ with the repository `.clang-format` file.
- Use `#pragma once` in headers and include what a file uses.
- Mark read-only accessors `[[nodiscard]]` and `noexcept` when the operation cannot fail.
- Keep public interfaces small. Put file-local helpers in an anonymous namespace.
- Comments should explain a non-obvious decision, not narrate the next line.
- Avoid magic gameplay values. Put shared tuning values in `Types.hpp` or a local named constant.

## Build and tests

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

Run `clang-tidy` and `clang-format` on changed C++ files when those tools are installed.

## Commits and pull requests

Use Conventional Commit subjects, for example `fix(save): preserve the previous record on write failure`.
Keep each pull request focused. Explain the reason for the change and list any gameplay behavior that changed.

Before opening a pull request, check that:

- [ ] The project builds with warnings treated as errors.
- [ ] All eight CTest executables pass.
- [ ] New behavior has a deterministic test where practical.
- [ ] README and presentation notes match the implementation.
- [ ] No copyrighted assets or unlicensed audio were added.
- [ ] Windows-specific behavior has been tested or is clearly marked as unverified.
