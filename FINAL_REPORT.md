# Final Delivery Report

Date: 2026-11-25  
Project: Space Survivor — Neon Edition 1.1.0

## Audit totals

| Severity | Findings |
|---|---:|
| Critical | 0 |
| High | 6 |
| Medium | 15 |
| Low | 7 |
| **Total** | **28** |

The findings are recorded as `AUDIT-001` through `AUDIT-028`. `AUDIT-019` records the dependency pin as a build safeguard; the other entries address source, test, or documentation changes. No critical defect was identified in the reviewed source.

## Files rewritten and created

| Area | Result |
|---|---|
| `include/` | Rewritten 16 headers; all use `#pragma once` and the `space_survivor` namespace. |
| `src/` | Rewritten 14 C++ source files. |
| `tests/` | Replaced the previous two-test set with eight standalone test programs. |
| `CMakeLists.txt` | Rewritten for C++17, exact SFML 2.6.1, warnings-as-errors, runtime assets, and CTest registration. |
| `.clang-format`, `.clang-tidy`, `.editorconfig` | Created. |
| `.github/workflows/ci.yml` | Created for Ubuntu 24.04 and Windows Server 2022. |
| `.vscode/` | Updated MinGW UCRT64 configure, build, test, run, and launch tasks. |
| `README.md`, `TEST_REPORT.md` | Rewritten in English. |
| `docs/PRESENTATION.md` | Replaced the former Indonesian presentation with an English presentation. |
| `AUDIT_REPORT.md`, `FINAL_REPORT.md` | Created. |
| `CHANGELOG.md`, `CONTRIBUTING.md` | Created. |

The 13 main classes are `Game`, `Player`, `Enemy`, `Projectile`, `Asteroid`, `PowerUp`, `Boss`, `ParticleSystem`, `Collision`, `HUD`, `Menu`, `SaveSystem`, and `AudioManager`.

## Tests: 2 to 8

| Test | Coverage | Result in this environment |
|---|---|---|
| `test_collision` | Circle and rectangle collision boundaries, asteroid damage limits | PASS with local SFML shim |
| `test_math` | Vector math, safe delta, wave progression, boss threshold, score saturation | PASS with local SFML shim |
| `test_player` | Movement, bounds, firing cooldown, health, pickup timers | PASS with local SFML shim |
| `test_enemy` | Basic/Fast/Heavy stats, movement, damage, off-screen retirement | PASS with local SFML shim |
| `test_boss` | Boss activation, firing, projectile cap, damage, defeat | PASS with local SFML shim |
| `test_powerup` | Pickup movement, collection, expiry | PASS with local SFML shim |
| `test_particles` | Pool capacity, expiry, clearing | PASS with local SFML shim |
| `test_save_system` | Missing/valid/corrupt/negative/trailing/maximum values, replacement | PASS in CTest; it uses the standard library only |

All 22 C++ translation units passed GCC 14.2 syntax checks using `-std=c++17 -Wall -Wextra -Wpedantic -Werror` and the temporary SFML API shim. All 14 application sources were compiled to object files and linked into a shim-backed executable; that executable was not launched. The eight test executables were built and run directly. CMake also configured and built the game and all test targets against temporary test-only SFML packages covering both supported target naming schemes. CTest passed 8/8 in both configurations. These package shims validate the build graph, warning policy, target selection, and test registration; they do not validate the real SFML library.

## Build and static checks

| Check | Result |
|---|---|
| C++17 syntax check with requested GCC warning flags | PASS with temporary shim |
| CMake configure/build and CTest | PASS with both temporary package target schemes; CTest 8/8 in each |
| Application object compilation and link | PASS with temporary shim; executable not launched |
| Eight test executable runs | PASS; see qualification above |
| `#pragma once` in every project header | PASS |
| Project namespace used throughout | PASS |
| Owning raw pointers / manual `new` and `delete` | None found |
| User-defined polymorphic classes without virtual destructors | None; no project polymorphic base class exists |
| erase-remove for inactive entity cleanup | PASS |
| CMake configure with actual SFML 2.6.1 | BLOCKED: dependency fetch could not resolve GitHub |
| Actual SFML application compile/link | NOT VERIFIED |
| clang-format / clang-tidy execution | NOT RUN: binaries are not installed in the environment |
| Ubuntu/Windows CI | Configured, not executed |

## Documentation consistency

The following project values were checked against the source and the documentation:

| Item | Agreed value |
|---|---|
| Starting health | 3 |
| Enemies | Basic, Fast, Heavy |
| Pickups | Health, Rapid Fire, Shield |
| Rapid Fire duration | 10 seconds |
| Shield duration | 8 seconds |
| Pickup lifetime | 12 seconds |
| Boss activation | 1,000 score or 120 seconds |
| Boss defeat bonus | 1,500 points |
| Wave duration | 30 seconds |
| High-score path | `saves/highscore.txt` |
| Controls | WASD / arrows / Space / Esc |
| Language | C++17 |
| Graphics library | SFML 2.6.1 |
| CMake minimum | 3.20 |
| Main classes | 13 |
| Registered test programs | 8 |

Documentation mismatches found during this pass: 3 (Rapid Fire duration, Shield duration, and the presentation language). All three were corrected. Checklist completeness is assessed as 8/10 before the pass and 10/10 after it; this is an internal checklist score, not an external review grade. The README, report, and presentation now use the same values.

## English-only and anti-AI pass

- Non-English strings in project UI, source comments, logs, and maintained documentation: none found in the final search.
- Emoji removed: 0 found in the final maintained documentation.
- Filler-word scan: 0 matches.
- Bold Markdown spans in README: 0.
- Comments removed: 0 verified; no preserved Git baseline was available for a defensible historical deletion count. Explanatory comments in the final code describe reasons or constraints; namespace-closing comments and `AUDIT` tags remain.
- The former `PRESENTATION_ID.md` was removed; the English file is `docs/PRESENTATION.md`.
- Subjective review: reads like a compact student game codebase with explicit constraints, not promotional copy. The audit comments are intentionally traceable and should be removed or retained according to the eventual review workflow.

## Quality delta

| Measure | Before | After |
|---|---:|---:|
| Standalone tests | 2 | 8 |
| CTest registrations | 0 / limited | 8 |
| GCC warning policy | Not consistent across targets | `-Wall -Wextra -Wpedantic -Werror` configured |
| Shared project namespace | Inconsistent | `space_survivor` |
| Bounded particles | No fixed cap | 1,200 |
| Bounded player projectiles | No fixed cap | 100 |
| Bounded hostile projectiles | No boss volley cap | 96 |
| Documentation | Mixed language / timer drift | English and values aligned |

A warning-count delta cannot be stated for the complete SFML build: that build did not run. The 22 translation units passed syntax checks with the requested warnings-as-errors flags against the shim, with zero diagnostics in those checks. The complete-build warning count remains unverified.

## Remaining limitations

1. SFML 2.6.1 could not be downloaded in this environment because DNS resolution for `github.com` failed. A local stub was used only for syntax and subsystem test execution.
2. The game has not been compiled, linked, or launched against the real SFML 2.6.1 package.
3. The Windows 10/11 path, runtime DLL copying, audio device behavior, font fallback, and full gameplay have not been manually tested.
4. clang-format, clang-tidy, and CI jobs are configured but were not executed here.
5. The save test covers normal file replacement and malformed content; it does not simulate power loss or two processes writing the same save simultaneously.
6. Score saturation is implemented, but there is no dedicated integration test for a score reaching `INT_MAX` through a full gameplay run.

## Windows manual checklist

| Check | Result |
|---|---|
| Configure from MSYS2 UCRT64 with `MinGW Makefiles` | NOT RUN |
| Build with warnings-as-errors | NOT RUN against real SFML |
| Confirm SFML runtime DLLs beside the executable | NOT RUN |
| Launch and close the main menu | NOT RUN |
| Test mouse and keyboard menu navigation | NOT RUN |
| Test WASD and arrow movement at all screen boundaries | NOT RUN |
| Test Space firing and Rapid Fire | NOT RUN |
| Test all three enemy types and asteroid collisions | NOT RUN |
| Test Health, Rapid Fire, and Shield pickups | NOT RUN |
| Verify pause/resume and restart | NOT RUN |
| Reach boss at 1,000 score or 120 seconds | NOT RUN |
| Confirm 1,500-point boss reward and Victory screen | NOT RUN |
| Restart the process and verify `saves/highscore.txt` | NOT RUN |
| Run with missing audio and font files | NOT RUN |
| Check frame rate on an 8 GB Windows laptop | NOT RUN |

## Next steps

1. Build on Windows 10 or 11 with MSYS2 UCRT64 or Visual Studio and the exact SFML 2.6.1 package.
2. Run `ctest --test-dir build --output-on-failure` and attach the output to the project submission.
3. Run `clang-format` and `clang-tidy` with the committed configuration; review every resulting change.
4. Complete the manual checklist and verify that SFML DLLs are copied to the executable directory.
