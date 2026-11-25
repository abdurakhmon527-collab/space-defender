# Test Report

Date: 2026-11-25  
Project version: 1.1.0  
Compiler available in the delivery environment: GCC 14.2.0  
CMake available in the delivery environment: 3.31.6

## Summary

All 22 C++ translation units (14 application sources and eight test sources) passed GCC 14.2.0 syntax checks with `-std=c++17 -Wall -Wextra -Wpedantic -Werror` against a temporary local SFML API shim. The 14 application source files were also compiled to object files and linked into a shim-backed executable. All eight revised test executables were built and passed using the shim; `test_save_system` was additionally built and run without the shim because it uses only the standard library.

The same temporary shim was exposed through a local test-only CMake package. With that package, CMake configured the project, built the game and all eight test targets with warnings treated as errors, and CTest passed 8/8. This verifies the project target graph and test registration. Both supported CMake target naming branches were exercised with the test package. The shim is not SFML and does not prove compatibility with SFML 2.6.1. Without the test package, CMake attempted to fetch the pinned `2.6.1` tag, but DNS resolution for GitHub failed. The application has not been compiled or linked against the real SFML library.

This is not a completed Windows QA pass.

## Test results

| Check | Result | Notes |
|---|---|---|
| C++ source syntax checks | PASS (shim) | 22 translation units; GCC 14.2.0; C++17; all requested GCC warning flags enabled as errors. |
| Application object build and link | PASS (shim) | All 14 application sources compiled to objects and linked against the temporary shim. The resulting executable was not launched. |
| `test_collision` | PASS (shim) | Collision edge cases, circle/rectangle contact, and asteroid damage limits. |
| `test_math` | PASS (shim) | Vector helpers, delta limits, waves, boss thresholds, and score saturation. |
| `test_player` | PASS (shim) | Movement, bounds, firing cooldown, health, and effect timers. |
| `test_enemy` | PASS (shim) | Enemy variants, stats, movement, damage, and off-screen cleanup. |
| `test_boss` | PASS (shim) | Activation, attack pattern, hostile projectile cap, health, and defeat state. |
| `test_powerup` | PASS (shim) | Movement, collection state, and 12-second lifetime. |
| `test_particles` | PASS (shim) | Capacity limits, expiry, clearing, and invalid emissions. |
| `test_save_system` | PASS | Missing, valid, malformed, negative, trailing data, `INT_MAX`, and replacement cases; test body uses the standard library only. |
| CMake configure/build with local test package | PASS (shim) | The game and all eight tests built with warnings-as-errors against a temporary test-only package. |
| CTest with local test package | PASS (shim) | 8/8 registered tests passed. |
| CMake configure with real SFML | BLOCKED | SFML was absent; FetchContent could not resolve `github.com`. |
| Full application compile/link against SFML 2.6.1 | NOT RUN | The local shim is not a substitute for the real SFML headers or libraries. |
| CTest using the real SFML build | NOT RUN | CTest passed against the local test package, but no generated build against real SFML was possible. |
| Windows 10/11 executable launch | NOT RUN | A Windows build environment was not available. |

## Automated coverage

The eight CTest-registered tests cover:

- Circle collision boundaries, circle-to-rectangle contact, and asteroid damage limits.
- Vector normalization, distance, clamping, delta-time limits, wave progression, boss activation, and score saturation.
- Player movement through supplied input, screen bounds, weapon cooldown, health, invulnerability, Rapid Fire/Shield expiry, and timer refresh.
- Basic, Fast, and Heavy enemy stats, movement, damage, and removal after leaving the screen.
- Boss activation, the three-shot attack pattern, projectile cap, damage, and defeat state.
- Power-up movement, collection, and the 12-second pickup lifetime.
- Particle count limits, trail capacity, expiry, clearing, and invalid emission counts.
- High-score missing-file behavior, valid reads, replacement, empty/corrupt/negative/trailing data, and `INT_MAX` boundaries.

The tests do not cover SFML rendering output, actual keyboard/mouse hardware input, audio playback, filesystem crash recovery under power loss, or an entire game session driven through the window event loop.

## Reproduction

With SFML 2.6.1 installed or reachable by CMake FetchContent:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure
```

The save test can also be compiled without SFML:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -Iinclude `
  tests/test_save_system.cpp src/SaveSystem.cpp -o test_save_system.exe
.\test_save_system.exe
```

## Manual Windows checklist

| Check | Result | Notes |
|---|---|---|
| Main menu opens and displays all five actions | NOT RUN | Requires a Windows graphical build. |
| Mouse hover, mouse click, and keyboard menu navigation | NOT RUN | Check focus and selection feedback. |
| WASD and arrow keys move in four directions | NOT RUN | Verify diagonal speed is normalized. |
| Ship remains inside the play area | NOT RUN | Test all four screen edges. |
| Holding Space fires at a controlled rate | NOT RUN | Check normal and Rapid Fire cooldowns. |
| Basic, Fast, and Heavy enemies spawn and can be destroyed | NOT RUN | Check rewards and health values. |
| Asteroids rotate, move, collide, and can be destroyed | NOT RUN | Test edge and player collisions. |
| Health, Rapid Fire, and Shield pickups work | NOT RUN | Verify 10-second and 8-second timers. |
| Damage and the 1.15-second invulnerability window work | NOT RUN | Test contact and hostile projectiles. |
| Pause freezes gameplay and Resume continues the run | NOT RUN | Background animation may continue. |
| Restart starts a clean run | NOT RUN | Check that entities and timers reset. |
| Boss appears at 1,000 score or 120 seconds | NOT RUN | Whichever threshold is reached first. |
| Boss health bar updates and defeat awards 1,500 points | NOT RUN | Confirm Victory state. |
| High score survives closing and reopening the game | NOT RUN | Inspect `saves/highscore.txt`. |
| Missing font/audio files do not stop gameplay | NOT RUN | Text labels may be hidden without a font. |
| Runtime SFML DLLs are present beside the Windows executable | NOT RUN | Check the Release output directory. |
| Game holds a stable 60 FPS on the target laptop | NOT RUN | Measure on the target hardware. |

## Environment notes

CMake 3.31.6 found no local SFML 2.6.1 package and attempted to fetch the pinned Git tag. The attempt failed because `github.com` could not be resolved from this environment. No claim is made that the revised application has compiled or run against the real SFML library. CI is configured for Ubuntu 24.04 and Windows Server 2022, but those jobs have not been executed from this session.
