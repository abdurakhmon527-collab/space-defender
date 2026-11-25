# Changelog

All notable changes to this project are recorded here. The format is based on Keep a Changelog and this project follows Semantic Versioning.

## [Unreleased]

## [1.1.0] - 2026-11-25

### Added
- Eight CTest executables for collision, math and game rules, player, enemy, boss, power-ups, particles, and save handling.
- Clang-Format, Clang-Tidy, EditorConfig, and Ubuntu/Windows CI configuration.
- English presentation notes, contribution guidance, and a verification report.

### Changed
- Moved project code into the `space_survivor` namespace.
- Reworked game update and collision responsibilities into shorter functions.
- Set Rapid Fire to 10 seconds and Shield to 8 seconds. Collecting either pickup refreshes its timer.
- Capped entity collections and reserved particle storage up front.
- High-score writes now replace the destination without deleting the previous file first.

### Fixed
- Prevented signed integer underflow when large damage values hit enemies, asteroids, or the boss.
- Rejected malformed high-score files with trailing data or out-of-range values.
- Removed enemies that leave the playable area.
- Capped boss projectile output and score accumulation.
- Made player movement update callable with explicit input for deterministic tests.
