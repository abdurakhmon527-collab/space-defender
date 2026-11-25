# Source Audit

Audit baseline: the project before the 1.1.0 rewrite. File locations below refer to that baseline; line numbers may differ after the rewrite.

Each finding has an `AUDIT` identifier. The corresponding implementation change carries the same identifier in the affected source or build file.

## Findings

### AUDIT-001

- **File:** `include/Types.hpp` and public headers
- **Severity:** MEDIUM
- **Category:** Style
- **Problem:** Public enums and project types were not consistently scoped in a project namespace.
- **Cause:** Global declarations made name collisions more likely as the project grew.
- **Fix:** Put public declarations and implementations in `space_survivor`; keep rules and configuration in nested namespaces.
- **Code:** `namespace space_survivor { enum class GameState { ... }; }`
- **Verify:** Search all project headers and sources for unqualified project declarations; compile every translation unit.

### AUDIT-002

- **File:** `src/Enemy.cpp`, `src/Asteroid.cpp`, `src/Boss.cpp`
- **Severity:** HIGH
- **Category:** Logic
- **Problem:** Subtracting arbitrary damage from a signed health counter could underflow.
- **Cause:** The code relied on the incoming damage value being smaller than the current health.
- **Fix:** Clamp before subtracting and mark the entity dead when health reaches zero.
- **Code:** `health_ = amount >= health_ ? 0 : health_ - amount;`
- **Verify:** The enemy, boss, and asteroid tests apply damage larger than the target's remaining health.

### AUDIT-003

- **File:** `src/SaveSystem.cpp`
- **Severity:** HIGH
- **Category:** Logic
- **Problem:** A valid numeric prefix could be accepted even when the save file contained trailing non-whitespace data.
- **Cause:** Numeric stream extraction alone does not prove that the complete file is valid.
- **Fix:** Parse the integer, reject negative and out-of-range values, consume whitespace, and require end-of-file.
- **Code:** `input >> std::ws; if (!input.eof()) return 0;`
- **Verify:** `test_save_system` covers empty, malformed, negative, trailing-data, and `INT_MAX` inputs.

### AUDIT-004

- **File:** `src/SaveSystem.cpp`
- **Severity:** HIGH
- **Category:** Memory / Data integrity
- **Problem:** Replacing a high-score file by removing the destination before the replacement was ready could lose the previous record on a failed write.
- **Cause:** The destination was exposed to an avoidable gap between deletion and replacement.
- **Fix:** Write and close a temporary file first, then replace the destination with a platform-appropriate rename operation.
- **Code:** `MoveFileExW(..., MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)` on Windows; `std::filesystem::rename` on POSIX.
- **Verify:** `test_save_system` checks that a valid record can be replaced and that invalid input does not become a record. Power-loss recovery is not simulated.

### AUDIT-005

- **File:** `include/Player.hpp`, `src/Player.cpp`
- **Severity:** MEDIUM
- **Category:** Testability
- **Problem:** Player movement read keyboard state directly, leaving no deterministic input path for unit tests.
- **Cause:** Input polling and movement calculation were coupled.
- **Fix:** Keep the keyboard-facing overload and add an overload that accepts a movement vector.
- **Code:** `void Player::update(float deltaTime, const sf::Vector2f& inputDirection) noexcept`
- **Verify:** `test_player` supplies input vectors without opening a window or polling hardware.

### AUDIT-006

- **File:** `src/Player.cpp`, `include/Types.hpp`
- **Severity:** MEDIUM
- **Category:** Logic / Documentation
- **Problem:** Pickup timing was not defined in one shared place and the effect durations could be described inconsistently.
- **Cause:** Timer values were local implementation details rather than documented game rules.
- **Fix:** Define Rapid Fire as 10 seconds and Shield as 8 seconds. Recollecting either pickup resets its timer rather than stacking duration.
- **Code:** `rapidFireTimer_ = config::RapidFireDuration;` and `shieldTimer_ = config::ShieldDuration;`
- **Verify:** `test_player` verifies both durations and timer refresh; README and presentation use the same values.

### AUDIT-007

- **File:** `src/Projectile.cpp`
- **Severity:** MEDIUM
- **Category:** Logic
- **Problem:** Projectile updates needed a consistent guard against invalid or unusually large frame deltas.
- **Cause:** An invalid delta could reverse movement or distort lifetime handling.
- **Fix:** Pass frame time through `math::safeDelta` before changing position or lifetime.
- **Code:** `const float dt = math::safeDelta(deltaTime);`
- **Verify:** `test_math` covers finite, non-finite, negative, and capped deltas; projectile tests verify movement and expiry.

### AUDIT-008

- **File:** `src/Enemy.cpp`
- **Severity:** MEDIUM
- **Category:** Logic / Performance
- **Problem:** Enemies that moved beyond the play area could remain alive and occupy spawn capacity.
- **Cause:** The update path did not retire every off-screen enemy.
- **Fix:** Mark enemies inactive after they pass the configured off-screen margin.
- **Code:** `if (position_.y > WindowHeight + OffscreenMargin) alive_ = false;`
- **Verify:** `test_enemy` moves an enemy beyond the boundary and checks its inactive state.

### AUDIT-009

- **File:** `src/ParticleSystem.cpp`
- **Severity:** MEDIUM
- **Category:** Performance
- **Problem:** Particle emission could grow the backing vector without a fixed upper bound.
- **Cause:** Every trail or explosion could allocate additional particles during a run.
- **Fix:** Reserve a fixed particle pool, cap emission at 1,200 particles, and erase expired particles.
- **Code:** `particles_.reserve(MaxParticles);` and capacity-checked emission.
- **Verify:** `test_particles` emits beyond the cap and checks the resulting size and clearing behavior.

### AUDIT-010

- **File:** `src/Boss.cpp`
- **Severity:** MEDIUM
- **Category:** Performance
- **Problem:** Repeated boss volleys could add hostile projectiles without checking the shared limit.
- **Cause:** The boss used its own firing path and could bypass the regular spawn limits.
- **Fix:** Refuse a three-projectile volley when it would exceed `config::MaxHostileProjectiles`.
- **Code:** `if (hostileProjectiles.size() + AttackAngles.size() > config::MaxHostileProjectiles) return;`
- **Verify:** `test_boss` checks the volley count; the game configuration sets the hostile projectile limit to 96.

### AUDIT-011

- **File:** `src/Collision.cpp`
- **Severity:** MEDIUM
- **Category:** Performance
- **Problem:** Circle collision checks performed a square-root calculation for each pair.
- **Cause:** The comparison used distance rather than squared distance.
- **Fix:** Compare squared center distance against the squared sum of the radii.
- **Code:** `distanceSquared <= combinedRadius * combinedRadius`
- **Verify:** `test_collision` covers overlap, touching, separation, and circle-to-rectangle contact.

### AUDIT-012

- **File:** `include/Math.hpp`, `include/GameRules.hpp`
- **Severity:** HIGH
- **Category:** Logic
- **Problem:** Non-finite frame values and very large survival times could propagate into timers or an unsafe integer conversion.
- **Cause:** Raw floating-point values were used before validation and narrowing conversion.
- **Fix:** Reject non-finite deltas, clamp frame time to 0.05 seconds, and calculate completed waves in double precision before clamping to the integer range.
- **Code:** `math::safeDelta` and the bounded `double` conversion in `rules::waveForTime`.
- **Verify:** `test_math` covers invalid deltas, normal wave boundaries, and large time input.

### AUDIT-013

- **File:** `src/AudioManager.cpp`
- **Severity:** MEDIUM
- **Category:** Logic / SFML
- **Problem:** Reusing one sound instance for all short effects could interrupt a sound that was already playing.
- **Cause:** A new `play()` call on the same `sf::Sound` changes that instance's playback state.
- **Fix:** Keep one `sf::Sound` instance per effect and load each buffer independently; missing files remain non-fatal.
- **Code:** `std::array<sf::Sound, SoundFiles.size()> sounds_;`
- **Verify:** Source syntax check and a Windows audio-device check; audible overlap has not been verified in this environment.

### AUDIT-014

- **File:** `src/Game.cpp`
- **Severity:** HIGH
- **Category:** Logic
- **Problem:** Repeated score awards could overflow a signed `int` during a long session.
- **Cause:** The score was incremented without checking the upper bound.
- **Fix:** Saturate the score at `std::numeric_limits<int>::max()`.
- **Code:** `score_ = rules::addScore(score_, points);` with saturation in the pure rules helper.
- **Verify:** `test_math` covers positive, negative, and `INT_MAX` score boundaries.

### AUDIT-015

- **File:** `src/Game.cpp`
- **Severity:** LOW
- **Category:** Style / Maintainability
- **Problem:** Frame coordination, entity updates, collision work, and cleanup were too closely grouped.
- **Cause:** One update path had to manage unrelated entity responsibilities.
- **Fix:** Split the work into `updatePlayer`, `updateEntities`, collision helpers, and `removeInactiveObjects`.
- **Code:** `update()` now coordinates those operations in order.
- **Verify:** Review function boundaries and run the subsystem test suite.

### AUDIT-016

- **File:** `src/Game.cpp` and gameplay source files
- **Severity:** LOW
- **Category:** Style
- **Problem:** Gameplay tuning values were scattered as inline literals.
- **Cause:** Spawn limits, timings, and gameplay rules were difficult to locate and compare with documentation.
- **Fix:** Move shared rules to `Types.hpp` and keep file-local tuning in named `constexpr` values.
- **Code:** `config::WaveDuration`, `config::BossTimeThreshold`, and named local constants.
- **Verify:** Review gameplay literals and compare README tables against the constants. Visual geometry constants remain local to drawing code.

### AUDIT-017

- **File:** Public headers, including `include/Player.hpp`
- **Severity:** LOW
- **Category:** Style
- **Problem:** Read-only accessors did not consistently communicate non-null, non-mutating access.
- **Cause:** Accessor declarations lacked `const`, `noexcept`, or `[[nodiscard]]` where those contracts applied.
- **Fix:** Mark read-only getters `[[nodiscard]]` and `noexcept` when their implementations cannot fail.
- **Code:** `[[nodiscard]] int getHealth() const noexcept;`
- **Verify:** Review declarations and compile with warnings treated as errors.

### AUDIT-018

- **File:** `CMakeLists.txt`
- **Severity:** MEDIUM
- **Category:** Build
- **Problem:** Compiler warnings were not enforced as errors on every target.
- **Cause:** Build options were not shared consistently between the game and test executables.
- **Fix:** Apply one interface target with `/W4 /WX` on MSVC and `-Wall -Wextra -Wpedantic -Werror` elsewhere.
- **Code:** `target_link_libraries(target PRIVATE space_survivor_options ...)`
- **Verify:** Inspect generated target commands on a machine with SFML installed; GCC syntax checks passed with the requested flags.

### AUDIT-019

- **File:** `CMakeLists.txt`
- **Severity:** LOW
- **Category:** Build / SFML
- **Problem:** The SFML version needed to be pinned so a future major-version API could not be selected accidentally.
- **Cause:** SFML 3 changes event and other APIs and is not source-compatible with this SFML 2.6.1 code.
- **Fix:** Require an exact 2.6.1 package or fetch the `2.6.1` Git tag.
- **Code:** `find_package(SFML 2.6.1 EXACT ...)` and `GIT_TAG 2.6.1`.
- **Verify:** Inspect the CMake dependency declaration. A real configure was attempted but blocked by unavailable network DNS.

### AUDIT-020

- **File:** `CMakeLists.txt`, `tests/`
- **Severity:** MEDIUM
- **Category:** Testing / Build
- **Problem:** The original test set covered only two small areas and was not registered as a full CTest suite.
- **Cause:** Gameplay subsystems had no separate executable-level regression tests.
- **Fix:** Add eight standalone tests and register each with `add_test()`.
- **Code:** `add_space_survivor_test(test_player ...)` and the corresponding seven target declarations.
- **Verify:** All eight test executables passed through CTest using a temporary local SFML package shim; this does not verify the real SFML dependency.

### AUDIT-021

- **File:** `docs/PRESENTATION_ID.md` (renamed to `docs/PRESENTATION.md`)
- **Severity:** MEDIUM
- **Category:** Documentation
- **Problem:** The presentation was not in the requested project language and its technical explanations needed to match the revised implementation.
- **Cause:** The project documentation was split between languages and had drifted from code-level timer rules.
- **Fix:** Replace it with an English presentation, add a testing section, and expand the question list.
- **Code:** Documentation-only change; all presentation text is in `docs/PRESENTATION.md`.
- **Verify:** Search documentation for non-English project text and compare all gameplay values with `Types.hpp`.

### AUDIT-022

- **File:** `.clang-format`, `.clang-tidy`, `.editorconfig`, `.github/workflows/ci.yml`
- **Severity:** LOW
- **Category:** Tooling
- **Problem:** The repository lacked shared formatting, static-analysis, editor, and cross-platform CI configuration.
- **Cause:** Local editor defaults could produce inconsistent changes and there was no committed build matrix.
- **Fix:** Add Google-based formatting rules with a 100-column limit, clang-tidy checks, EditorConfig, and Ubuntu/Windows CI jobs.
- **Code:** New repository configuration files.
- **Verify:** Validate configuration text and inspect the CI matrix. CI has not run from this session.

### AUDIT-023

- **File:** `src/Game.cpp`
- **Severity:** HIGH
- **Category:** Logic / Data integrity
- **Problem:** A new record could be lost if the application closed before the run reached Game Over.
- **Cause:** High-score persistence happened at the end of a run rather than when the record was first exceeded.
- **Fix:** Save immediately when the active score exceeds the stored high score, and save again when the run ends.
- **Code:** `if (score_ > highScore_) { highScore_ = score_; saveHighScore(highScore_); }`
- **Verify:** Review the active update path and run `test_save_system`; a process-kill integration test is not included.

### AUDIT-024

- **File:** `src/Game.cpp`
- **Severity:** MEDIUM
- **Category:** Performance
- **Problem:** Holding the fire key could create player projectiles without a fixed in-flight limit.
- **Cause:** Continuous firing was not gated by the projectile collection size.
- **Fix:** Cap the player projectile collection at 100 before adding a two-shot volley.
- **Code:** `playerProjectiles_.size() + 2U <= MaxPlayerProjectiles`
- **Verify:** Inspect the fire path and the constant; full input-loop stress testing remains a Windows manual check.

### AUDIT-025

- **File:** `include/GameRules.hpp`
- **Severity:** MEDIUM
- **Category:** Logic / Requirements
- **Problem:** Boss activation must not depend on only one progression metric.
- **Cause:** A run that scores slowly still needs a defined boss encounter, while a high-scoring run must not wait for the timer.
- **Fix:** Activate the boss at 1,000 points **or** 120 seconds, whichever happens first.
- **Code:** `score >= BossScoreThreshold || survivalSeconds >= BossTimeThreshold`
- **Verify:** `test_math` checks score and time boundaries separately.

## Audit notes

- The rewritten project uses SFML 2.6.1 API conventions. No SFML 3-only API was intentionally introduced.
- The actual SFML headers and libraries were unavailable during this audit. C++ syntax checks used a temporary local test shim; see `TEST_REPORT.md`.
- No project class uses owning raw pointers or manual `new`/`delete`. The project uses value-owned containers and RAII.
- There are no user-defined polymorphic base classes, so no project virtual destructor is required.
- Erasure of inactive entities uses the erase-remove idiom.

### AUDIT-026

- **File:** `CMakeLists.txt`
- **Severity:** MEDIUM
- **Category:** Build / SFML
- **Problem:** The post-build runtime copy handled lowercase SFML target names but not the namespaced imported targets accepted by the package-detection branch.
- **Cause:** Link-target detection and runtime-target copying used different target lists.
- **Fix:** Set the runtime target list in both package branches and iterate over that list when copying files.
- **Code:** `set(SPACE_SURVIVOR_SFML_RUNTIME_TARGETS SFML::Graphics SFML::Window SFML::System SFML::Audio)` in the namespaced branch.
- **Verify:** Inspect both branches and the shared copy loop. A real Windows package build remains unverified.

### AUDIT-027

- **File:** `src/Game.cpp`
- **Severity:** LOW
- **Category:** SFML / Logic
- **Problem:** The main loop could perform one final update and draw after the window-close event had closed the render window.
- **Cause:** The loop checked window state only at the start of the next iteration.
- **Fix:** Check `window_.isOpen()` immediately after event processing and leave the loop before updating or drawing.
- **Code:** `if (!window_.isOpen()) break;`
- **Verify:** Review the event-to-frame order; actual close behavior still needs a Windows GUI check.

### AUDIT-028

- **File:** `tests/test_player.cpp`
- **Severity:** LOW
- **Category:** Test / Maintainability
- **Problem:** The player test's `main()` contained several independent scenarios in one function longer than 50 lines.
- **Cause:** Movement, firing, pickup timing, and damage assertions had accumulated in one test entry point.
- **Fix:** Split the scenarios into focused test functions and keep `main()` as a short runner.
- **Code:** `testMovementAndBounds`, `testShootingCooldown`, `testPowerUpTimers`, and `testDamageAndInvulnerability`.
- **Verify:** Build with warnings-as-errors and run `test_player` through CTest.
