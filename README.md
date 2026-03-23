# MemoryGame

A cross-platform memory matching game built with modern C++ and SFML, featuring a modular engine architecture, comprehensive automated testing, and an extensible end-to-end testing framework.

## Overview

![MemoryGame screenshot showing a 4x4 card grid with some cards flipped to reveal colored tiles with letter labels](MemoryGame/docs/screenshot.png)

**Gameplay**: Players flip cards two at a time to find matching pairs on an expanding grid. Start with a 4x4 board (8 pairs) and advance through progressively larger levels (up to 8x8) as you complete each round.

## Quick Start

```bash
# Configure and build
cmake -S . -B Build -DMEMORYGAME_BUILD_TESTS=ON
cmake --build Build

# Run the game
./Build/bin/MemoryGame

# Run tests
ctest --test-dir Build

# Run E2E tests
./Build/bin/memorygame_e2e
```

## Project Structure

- **[MemoryGame/](MemoryGame)**: Game logic and UI (Card, GameModel, GameScene)
- **[Engine/](Engine)**: Reusable runtime (Scene, Game loop, DelayedAction)
- **[E2EFramework/](E2EFramework)**: App-agnostic end-to-end testing framework
- **[Tests/](Tests)**: Unit and integration tests with game drivers

## Table of Contents

- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Module Details](#module-details)
- [Engine Overview](#engine-overview)
- [Game Rules & State](#gameplay-rules)
- [Building & Running](#build-and-run)
- [Testing](#testing)
- [Extending the Game](#tuning-guide)

## Module Details

## What This Module Builds

## Module Details

### What This Module Builds

- `MemoryGameCore` (library)
  - Source files:
    - [Source/Card.cpp](Source/Card.cpp)
    - [Source/GameModel.cpp](Source/GameModel.cpp)
    - [Source/GameScene.cpp](Source/GameScene.cpp)
  - Public headers are under [Include](Include)
  - Links against the shared `Engine` target

- `MemoryGame` (executable)
  - Entry point: [Source/main.cpp](Source/main.cpp)
  - Links against `MemoryGameCore`

## Folder Layout

- [Include/Card.h](Include/Card.h): visual and interaction model for one card
- [Include/GameModel.h](Include/GameModel.h): game rules and progression state
- [Include/GameScene.h](Include/GameScene.h): scene/controller + UI wiring
- [Include/GameConstants.h](Include/GameConstants.h): window constants
- [Source](Source): implementations and entry point

## Related Project Areas

- **Engine**: [../Engine](../Engine) — Shared runtime and scene management
- **E2EFramework**: [../E2EFramework](../E2EFramework) — Reusable testing framework
- **Tests**: [../Tests](../Tests) — Game-specific test suite and drivers

## Requirements

- C++17 compiler
- CMake 3.16+
- SFML 2.5+ (auto-fetched if not found)
- GoogleTest (auto-fetched for tests)

## Build & Run
configure/build from the repository root.

```bash
cmake -S . -B Build -DMEMORYGAME_BUILD_TESTS=ON
cmake --build Build --target MemoryGame
./Build/bin/MemoryGame
```

Notes:

- The root CMake setup finds SFML locally or fetches SFML 2.6.2 automatically.
- On macOS, the executable is configured as a regular binary (not an app bundle).
- Debug builds use `-DCMAKE_BUILD_TYPE=Debug`; Release builds use `-DCMAKE_BUILD_TYPE=Release`

## Testing

### Unit Tests

```bash
# Build and run all tests
cmake --build Build --target memorygame_tests
ctest --test-dir Build --verbose
```

### End-to-End Tests

```bash
# Headless (default, no window required)
./Build/bin/memorygame_e2e

# Headed (requires display)
cmake -B Build -DMEMORYGAME_HEADED_E2E=ON
cmake --build Build --target memorygame_e2e
./Build/bin/memorygame_e2e

# Run specific category
./Build/bin/memorygame_e2e --category=Smoke

# Repeat tests for flakiness detection
MEMORYGAME_E2E_REPEAT=10 ./Build/bin/memorygame_e2e

# Generate animated GIF with logs overlay
./Tests/Tools/run_e2e_gif.sh
```

For more details, see [E2EFramework/README.md](E2EFramework/README.md).

## Extending the Game

## Engine Overview

`MemoryGame` runs on the shared Engine module in [../Engine](../Engine).

Key Engine types used by this module:

- [../Engine/Include/Engine/Game.h](../Engine/Include/Engine/Game.h)
- [../Engine/Include/Engine/Scene.h](../Engine/Include/Engine/Scene.h)
- [../Engine/Include/Engine/DelayedAction.h](../Engine/Include/Engine/DelayedAction.h)

### Engine::Game

[../Engine/Include/Engine/Game.h](../Engine/Include/Engine/Game.h) defines the
runtime host:

- Creates and owns the SFML window in `Headed` mode.
- Can run in `Headless` mode (no window) for automated scenarios.
- Owns current and pending scenes and applies scene transitions safely.
- Exposes `run(startScene)` for real-time loop execution.
- Exposes `step(dt)` for deterministic, externally driven progression.
- Exposes `quit()`, `isRunning()`, `hasWindow()`, and `getWindow()`.

Loop behavior in [../Engine/Source/Engine/Game.cpp](../Engine/Source/Engine/Game.cpp):

1. Poll events and forward to current scene.
2. Apply pending scene transition.
3. Update current scene.
4. Apply pending scene transition again (if requested during update).
5. Draw current scene, optionally capture frame artifacts, then display.

### Engine::Scene

[../Engine/Include/Engine/Scene.h](../Engine/Include/Engine/Scene.h) defines a
minimal scene interface with virtual hooks:

- `handleEvent(const sf::Event&)`
- `update(float dt)`
- `draw(sf::RenderWindow&)`

`GameScene` in this module derives from `Engine::Scene` and implements all
three hooks.

### Engine::DelayedAction

[../Engine/Include/Engine/DelayedAction.h](../Engine/Include/Engine/DelayedAction.h)
is a small one-shot timer utility:

- `start(duration, callback)` schedules a callback
- `update(dt, canAdvance)` progresses time and fires callback when elapsed
- `cancel()` aborts pending action

`GameModel` uses two delayed actions:

- mismatch reveal delay before flipping unmatched cards down
- win delay before automatically advancing to the next level

### Frame Capture Support

In headed mode, the Engine can save per-frame PNGs when
`MEMORYGAME_CAPTURE_FRAMES_DIR` is set in the environment.

Implementation is in
[../Engine/Source/Engine/Game.cpp](../Engine/Source/Engine/Game.cpp) via
`captureFrameIfRequested()`.

## Gameplay Rules

Rules are implemented in [Source/GameModel.cpp](Source/GameModel.cpp).

1. The board starts at 4x4 (8 pairs).
2. Players flip two cards at a time.
3. If both cards match:
   - both become permanently matched
   - `matchedPairs` increases
4. If cards do not match:
   - state switches to `ShowingMismatch`
   - after `mismatchDelay` (default `0.9s`) both cards flip back down
5. Completing all pairs triggers `Won`.
6. After `winDelay` (default `1.2s`), level advances automatically.
7. Grid grows by +2 per side each level (4x4 -> 6x6 -> 8x8), capped by
   `maxGridSize`.
8. `moves` counts each completed 2-card attempt.

## Runtime State Model

[Include/GameModel.h](Include/GameModel.h) exposes:

- `GameState`:
  - `Playing`
  - `ShowingMismatch`
  - `Won`
- `GameModelConfig`:
  - `initialGridSize` (default `4`)
  - `maxGridSize` (default `8`)
  - `mismatchDelay` (default `0.9f`)
  - `winDelay` (default `1.2f`)

Core API:

- `tryFlip(index)`: attempts to flip a card (returns `false` if invalid)
- `update(dt)`: advances animations and delayed actions
- `reset()`: resets to level 1 and initial grid
- read accessors for cards, moves, level, grid size, matched pairs, state

## Card Rendering And Animation

Implemented in [Source/Card.cpp](Source/Card.cpp):

- Flip animation uses an interpolated parameter `flipT` and cosine-based width
  scaling.
- Card front color is derived from pair id using HSV -> RGB conversion.
- Matched cards are brightened.
- Face labels use an alphanumeric symbol set (`ABCDEFGHJKLMNPQRSTUVWXYZ23456789`)
  to avoid ambiguous characters.

Public behavior is defined in [Include/Card.h](Include/Card.h):

- `flipUp()`, `flipDown()`
- `setMatched(bool)`
- `contains(point)` and `getBounds()` for hit testing
- `update(dt)` and `draw(window, font)`

## Scene, Input, And UI

Implemented in [Source/GameScene.cpp](Source/GameScene.cpp) and declared in
[Include/GameScene.h](Include/GameScene.h).

Responsibilities:

- Handles left mouse clicks.
- Restarts game when restart button is clicked.
- Delegates card clicks to `GameModel::tryFlip`.
- Updates HUD text:
  - moves
  - level + current grid size
  - win/level-complete status message
- Draws board, title, stats, restart button, and status.

Font loading behavior:

- Attempts common macOS font paths:
  - `/System/Library/Fonts/Supplemental/Arial.ttf`
  - `/System/Library/Fonts/Helvetica.ttc`
- If no font is available, gameplay still runs but text UI is skipped.

## Entry Point

[Source/main.cpp](Source/main.cpp) creates and runs the engine game loop:

- title: `Memory Game`
- size from [Include/GameConstants.h](Include/GameConstants.h): `600x630`
- initial scene: `GameScene`

## Extending the Game

### Custom Difficulty

Modify `GameModelConfig` to change difficulty and pacing:

```cpp
GameModelConfig cfg;
cfg.initialGridSize = 2;    // Start smaller
cfg.maxGridSize = 6;         // Stop earlier
cfg.mismatchDelay = 0.5f;   // Flip back faster
cfg.winDelay = 0.8f;        // Advance sooner

GameScene app(game, cfg);
```

### Adding New Features

1. **New Card Types**: Extend `Card` class for special behaviors
2. **New Game Modes**: Create new `GameScene` derivatives
3. **UI Customization**: Override `GameScene::draw()` and `handleEvent()`
4. **AI Player**: Implement using the E2E framework driver pattern

## Tuning Guide

## Tuning Guide

You can tune difficulty and pacing by changing `GameModelConfig` defaults in
[Include/GameModel.h](Include/GameModel.h) or by passing custom config to
`GameScene`.

Common tweaks:

- Faster rounds: reduce `mismatchDelay` and `winDelay`
- Longer progression: increase `maxGridSize`
- Different opening difficulty: change `initialGridSize`

## Interaction Constraints

`tryFlip` intentionally rejects input when:

- game state is not `Playing`
- selected card is already matched
- selected card is already face up
- selected card is animating
- selected index is already one of the active selections

This keeps gameplay deterministic and avoids race conditions during animations
and delayed callbacks.