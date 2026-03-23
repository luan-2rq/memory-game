# MemoryGame Module

This folder contains the playable Memory Game application and its core gameplay
library.

## What This Module Builds

Defined in [CMakeLists.txt](CMakeLists.txt):

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

## Build And Run

This module depends on targets created by the repository root CMake project, so
configure/build from the repository root.

```bash
cmake -S . -B Build -DMEMORYGAME_BUILD_TESTS=ON
cmake --build Build --target MemoryGame
./Build/bin/MemoryGame
```

Notes:

- The root CMake setup finds SFML locally or fetches SFML 2.6.2 automatically.
- On macOS, the executable is configured as a regular binary (not an app
  bundle).

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

## Related Project Areas

- Engine runtime: [../Engine](../Engine)
- End-to-end framework: [../E2EFramework](../E2EFramework)
- Automated tests: [../Tests](../Tests)