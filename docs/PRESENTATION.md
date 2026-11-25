<!-- AUDIT-021: Keep presentation language and project terminology consistent with the README. -->

# Space Survivor — Neon Edition

## 1. Opening

Good morning, everyone. Today I am presenting my C++ programming project, Space Survivor — Neon Edition. It is a 2D arcade survival shooter built with C++17 and SFML 2.6.1.

The player pilots a spaceship, destroys incoming enemies, avoids asteroids, collects equipment, and tries to survive until the Sector Warden appears. The project is intended to show how basic programming concepts can be used in a small interactive application.

## 2. Technologies

- C++17 for the game logic and object-oriented design.
- SFML 2.6.1 for the window, graphics, keyboard and mouse input, and optional audio.
- CMake 3.20 or newer for configuration and building.
- `std::vector` for projectiles, enemies, asteroids, pickups, stars, and particles.
- File streams and `std::filesystem` for high-score persistence.

The ships, asteroids, pickups, and particle effects are drawn from SFML shapes. No external sprite sheet is required.

## 3. Gameplay

The player starts with 3 health points. W, A, S, D or the arrow keys move the ship. Holding Space fires lasers. Escape pauses the game or resumes it from the pause screen.

There are three enemy types: Basic, Fast, and Heavy. They differ in speed, health, contact damage, and score value. Asteroids move and rotate through the play area. Player projectiles can destroy enemies and asteroids.

The game has three pickups. Health restores one health point, Rapid Fire reduces the weapon cooldown for 10 seconds, and Shield blocks damage for 8 seconds. Collecting Rapid Fire or Shield again resets that effect's timer rather than adding time to it.

A new wave starts every 30 seconds. The boss appears when the score reaches 1,000 or the survival timer reaches 120 seconds. Defeating it adds 1,500 points and moves the game to the Victory screen.

## 4. Program structure

The project has 13 main classes:

- `Game` owns the window, game state, update loop, entity collections, and system coordination.
- `Player` handles movement, health, shooting, and collected effects.
- `Enemy`, `Asteroid`, `Projectile`, `PowerUp`, and `Boss` represent gameplay entities.
- `ParticleSystem` manages temporary visual effects.
- `Collision` contains reusable collision checks.
- `HUD` and `Menu` draw the interface and handle menu selection.
- `SaveSystem` reads and writes the high score.
- `AudioManager` loads optional sound effects and music.

The classes use composition and enum-based enemy variants. The project does not use a base enemy class because a deep inheritance structure would add complexity without improving the current gameplay.

## 5. C++ concepts

Variables and data types store positions, speed, health, score, and elapsed time. Conditional statements select enemy behavior, handle collisions, and change game states. Loops process input events and update or draw entity collections.

Classes and objects group related state and operations. Private members provide encapsulation. `std::vector` stores collections whose sizes change during a run. References are used when functions need access to existing objects without copying them.

The save system demonstrates file input and output. Collision checks use vector distance and object radii. Delta time keeps movement and timers independent of the frame rate. The project uses standard containers and RAII; it does not use owning raw pointers or manual `new` and `delete`.

## 6. Testing and quality control

CMake registers eight test executables. They check collision boundaries, mathematical rules, player behavior, enemy behavior, boss behavior, pickup lifetime, particle limits, and save-file edge cases.

All eight test programs were built and passed locally. Seven SFML-dependent tests used a temporary local API shim because SFML 2.6.1 was not installed and GitHub could not be reached during CMake dependency fetching. The save-system test was built and run without the shim. This does not prove compatibility with the real SFML library, and it is not a completed Windows QA pass.

## 7. Closing

Space Survivor is a practical example of C++17 in a graphical application. It combines input handling, object-oriented code, standard containers, collision detection, file storage, and a real-time game loop. Building and testing the project also shows why clear class responsibilities and repeatable test cases matter.

Thank you for listening.

## Questions

### Why use `std::vector`?

The number of enemies, projectiles, asteroids, pickups, and particles changes during a run. A vector provides dynamic storage and works well with the erase-remove pattern used to remove inactive objects.

### What does delta time do?

Delta time is the elapsed time between two frames. Multiplying speed by delta time makes movement consistent when the frame rate changes.

### How does collision detection work?

For two circular objects, the program compares the squared distance between their centers with the square of their combined radii. This avoids a square-root calculation for the common circle test.

### Why use separate classes?

Each class owns a focused part of the program. For example, `Player` handles player state while `SaveSystem` handles persistent data. This makes individual systems easier to read and test.

### Why are enemy types stored in an enum?

The three enemy types share the same update and drawing structure, but have different statistics. An enum and one `Enemy` class represent those differences without adding subclasses that the game does not currently need.

### How is the highest score protected from malformed data?

The loader checks that the file contains one non-negative integer within the range of `int`. Extra non-whitespace text and values outside that range are rejected and treated as zero.

### What happens when a power-up is collected twice?

Rapid Fire and Shield timers are refreshed to their full duration. The remaining time is not added to the new duration, which keeps the effect length predictable.

### What is tested automatically, and what still needs a person?

The tests cover deterministic logic such as collision boundaries, timer rules, entity health, particle caps, and save parsing. Rendering, physical input, sound playback, and complete Windows gameplay still need manual testing.

### What would be a sensible next improvement?

A short run-history file would be a useful addition after Windows behavior is verified. It could record several recent scores without changing the main gameplay architecture.
