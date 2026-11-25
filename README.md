# Space Survivor — Neon Edition

Space Survivor is a 2D arcade survival shooter written in C++17 with SFML 2.6.1. Fly a ship, shoot three enemy types, avoid asteroids, collect equipment, and fight the Sector Warden. The game uses procedural SFML shapes; no sprite downloads are required.

## Build

### Requirements

- CMake 3.20 or newer
- A C++17 compiler: MSVC 2019 or newer, or GCC 10 or newer
- Git, when CMake needs to fetch SFML
- SFML 2.6.1 (CMake fetches the `2.6.1` tag if an exact local version is not found)

On Windows, use the x64 Native Tools Command Prompt for Visual Studio or the MSYS2 UCRT64 terminal. The compiler, Git, and CMake must be available on `PATH`. The VS Code tasks are set up for the MSYS2 MinGW-w64 single-configuration generator.

### Configure and compile

For MSYS2 UCRT64, run these commands from the repository root in PowerShell or the VS Code terminal:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

CMake first looks for SFML 2.6.1. If it is missing, it downloads that release and builds the Graphics, Window, System, and Audio modules. The first configure therefore needs network access and the native dependencies required by SFML.

Launch the game:

```powershell
.\build\SPACE_SURVIVOR.exe
```

For Visual Studio, omit `-G "MinGW Makefiles"`, retain `--config Release` when building, and launch `build\Release\SPACE_SURVIVOR.exe`.

### Run the tests

```powershell
ctest --test-dir build --output-on-failure
```

There are eight registered test executables. They do not open a game window.

## Controls

| Input | Action |
|---|---|
| W / A / S / D | Move |
| Arrow keys | Move |
| Space | Fire while held |
| Esc | Pause or resume |
| Up / Down in menus | Change selection |
| Enter / Space in menus | Activate selection |
| Mouse | Hover over and click menu items |
| M on the Settings screen | Toggle mute |

## Gameplay

The player starts with 3 health points. Contact with an enemy, an asteroid, or a hostile projectile removes health. After taking damage, the player is invulnerable for 1.15 seconds. A Shield pickup blocks damage while its timer is active.

The three enemy types have different movement speeds, health, contact damage, and score values:

| Enemy | Health | Contact damage | Score |
|---|---:|---:|---:|
| Basic | 2 | 1 | 100 |
| Fast | 1 | 1 | 150 |
| Heavy | 6 | 2 | 300 |

Asteroids rotate and drift across the play area. Player lasers can destroy them for 25 points. Enemy destruction has a 15 percent pickup drop chance.

Waves advance every 30 seconds. Spawn intervals decrease as the wave number rises, with lower limits so the game does not create an unlimited update rate. The boss appears when the score reaches 1,000 or survival time reaches 120 seconds, whichever happens first. Defeating the boss awards a 1,500-point bonus and opens the Victory screen.

There are three pickups:

| Pickup | Effect | Duration |
|---|---|---:|
| Health | Restores 1 health point, up to 3 | Instant |
| Rapid Fire | Reduces the weapon cooldown | 10 seconds |
| Shield | Prevents damage | 8 seconds |

Collecting Rapid Fire or Shield again resets that effect to its full duration; timers do not stack. A pickup expires 12 seconds after it spawns if it is not collected.

The highest score is stored as a plain non-negative integer in `saves/highscore.txt`. Missing, malformed, negative, or out-of-range save data loads as zero. The save is updated when a new record is reached and when a run ends.

## Project layout

```text
SPACE_SURVIVOR/
├── CMakeLists.txt
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
├── include/                 Public class and utility declarations
├── src/                     Game and entity implementations
├── tests/                   Eight standalone test programs
├── docs/PRESENTATION.md      University presentation notes
├── assets/                  Optional font and audio folders
├── saves/                   Runtime high-score file
├── .github/workflows/ci.yml Ubuntu and Windows build jobs
├── .clang-format
├── .clang-tidy
└── .editorconfig
```

The 13 main classes are `Game`, `Player`, `Enemy`, `Projectile`, `Asteroid`, `PowerUp`, `Boss`, `ParticleSystem`, `Collision`, `HUD`, `Menu`, `SaveSystem`, and `AudioManager`.

## Design notes

All project code lives in the `space_survivor` namespace. Entity collections are value-owned `std::vector` instances. The game loop updates movement, timers, collision checks, and particles with delta time. The frame delta is capped at 0.05 seconds so a long pause in the process does not move entities across the screen in one update.

`Game` coordinates the run. Entity classes keep their own state and rendering code. `Collision` and the `math` namespace contain reusable calculations. `rules` contains deterministic wave, spawn interval, and boss threshold rules. `SaveSystem` handles the high-score file separately from gameplay.

The project uses composition and enum-based enemy variants instead of a polymorphic inheritance tree. That keeps the number of moving parts low for a first-year C++ course. The code demonstrates constructors, encapsulation, standard containers, references, file I/O, event handling, collision algorithms, and delta-time updates. It does not claim to demonstrate runtime polymorphism.

## Testing

The CMake test targets are:

- `test_collision` — circle and rectangle boundaries, asteroid damage limits
- `test_math` — vector calculations, frame delta limits, wave progression, boss threshold, and score saturation
- `test_player` — movement, bounds, shooting cooldown, damage, and pickup timers
- `test_enemy` — enemy stats, movement, damage, and off-screen cleanup
- `test_boss` — activation, firing pattern, damage, and defeat
- `test_powerup` — movement, collection, and expiry
- `test_particles` — particle lifetime and capacity limit
- `test_save_system` — missing, valid, malformed, negative, maximum, and replacement cases

Build and run all tests with the commands in the Build section. See `TEST_REPORT.md` for what has and has not been executed in the delivery environment. `AUDIT_REPORT.md` records the source review and fix IDs. `FINAL_REPORT.md` lists the delivered files and remaining verification gaps.

## Optional assets

The game draws ships, enemies, asteroids, pickups, particles, and interface panels with SFML geometry. It does not need texture files.

For a custom font, put a licensed TrueType font at `assets/fonts/Orbitron-Regular.ttf`. If that file is not present, the game checks common Windows and Linux font paths. If no font can be loaded, gameplay still runs, but menu and HUD labels are hidden.

Optional audio files can be placed in `assets/audio/`:

```text
shoot.wav
explosion.wav
damage.wav
powerup.wav
menu.wav
background.ogg
```

Each file is checked independently. Missing audio does not stop the game. The repository does not redistribute audio or font files.

## Limitations

- No Windows graphical QA pass has been completed in the current development environment.
- Visual rendering, keyboard focus, audio-device behavior, and the full boss encounter still need manual testing on a Windows 10 or Windows 11 machine.
- The default art is procedural. There are no sprite animations or externally supplied textures.
- Enemy movement is direct steering toward the player; there is no pathfinding.
- The current save format stores one high-score integer and does not keep a run history.
- Audio playback is optional and depends on files supplied by the user.

## License

The project source is provided for educational use. SFML is distributed under its own licence. Any font or sound file added to the asset folders must have a licence that permits the intended use and redistribution. No commercial game art or music is included.
