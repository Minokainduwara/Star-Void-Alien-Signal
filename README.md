# Star Void: Alien Signal

A fast-paced 2D side-scrolling space shooter built with **C17** and **raylib**. Inspired by classic arcade shooters like Space Impact, featuring modern mechanics, smooth animations, and a unique weapon evolution system.

The player controls the last remaining spaceship in a mysterious region of space called the Star Void. A strange alien signal has awakened hostile alien forces, and the player must survive endless waves of enemies while collecting energy fragments to upgrade the ship.

## Features

### Gameplay
- **Smooth touch & keyboard controls** - Drag to move, tap to fire
- **4-tier weapon evolution** - Basic Laser → Double Laser → Plasma Spread → Void Cannon
- **5 enemy types** - Scouts, Fighters, Hunters, Shielded enemies, and multi-phase Bosses
- **6 power-ups** - Energy Crystals, Health, Shield, Speed Boost, Drone Companion, Void Blast
- **Combo scoring system** - Chain kills for multiplier bonuses
- **Screen shake & particle effects** - Explosions, engine trails, glowing bullets

### Game Modes
- **Campaign Mode** - 10 waves with story progression and boss fights
- **Survival Mode** - Endless waves with increasing difficulty
- **Boss Rush Mode** - Fight only boss enemies (unlocked after campaign)

### Systems
- **Weapon Evolution** - Weapons evolve automatically based on kill count
- **Upgrade Shop** - 6 purchasable upgrades (3 levels each) using energy fragments
- **Shield System** - Absorbs damage before health, with passive regeneration upgrade
- **Drone Companion** - Auto-firing orbital drone
- **Alien Signal Story** - Narrative revealed at key campaign milestones

### Visual Style
- Neon sci-fi theme with cyan/blue/purple color palette
- All graphics rendered procedurally (no external assets needed)
- Glow effects, pulsing power-ups, animated enemies
- Scrolling starfield with parallax and twinkling stars

## Requirements

- **CMake** 3.16+
- **raylib** 6.0+
- **C17 compatible compiler** (Clang, GCC, MSVC)

### Installing raylib

**macOS (Homebrew):**
```bash
brew install raylib
```

**Ubuntu/Debian:**
```bash
sudo apt install libraylib-dev
```

**Windows:**
Download from [raylib.com](https://www.raylib.com/)

## Building

```bash
# Clone the repository
git clone <repo-url>
cd StarVoidAlienSignal

# Configure and build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./StarVoidAlienSignal
```

Or on macOS simply open the .app bundle:
```bash
open StarVoidAlienSignal.app
```

## Controls

| Input | Action |
|-------|--------|
| **WASD / Arrow Keys** | Move ship |
| **Space / Enter** | Fire weapon |
| **ESC / P** | Pause / Resume |
| **M** | Return to menu (while paused) |
| **1-6** | Purchase upgrades (in upgrade menu) |
| **Mouse drag (left 70%)** | Move ship |
| **Mouse click (right 30%)** | Fire weapon |
| **Touch drag** | Move ship |
| **Touch tap (right side)** | Fire weapon |

## Project Structure

```
StarVoidAlienSignal/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── .gitignore
├── src/
│   ├── game.h              # Master header: types, constants, declarations
│   ├── main.c              # Entry point, game loop, input
│   ├── game.c              # State machine, update/draw dispatch
│   ├── player.c            # Player ship system
│   ├── bullet.c            # Object-pool bullet system
│   ├── weapon.c            # Weapon evolution and firing patterns
│   ├── enemy.c             # Enemy types, AI behaviors
│   ├── collision.c         # Collision detection
│   ├── particles.c         # Particle effects + starfield
│   ├── powerup.c           # Power-up spawning and collection
│   ├── level.c             # Wave spawning, difficulty scaling
│   ├── story.c             # Narrative system
│   ├── audio.c             # Procedural sound generation
│   └── ui.c                # Menus, HUD, touch controls
└── saves/                  # Save data (auto-created)
```

## Architecture

- **Entity pools**: Fixed-size arrays for bullets (128), enemies (64), particles (512), power-ups (16) - no dynamic allocation during gameplay
- **State machine**: Game states (Menu, Playing, Paused, GameOver, Upgrade, Story, Win) with clean transitions
- **Modular design**: Each system in its own .c file with clear interfaces
- **Desktop first, Android ready**: Touch controls built-in, raylib's Android support requires only NDK setup

## Android Porting

To build for Android:

1. Install Android NDK
2. Use raylib's Android CMake toolchain
3. Add the `android/` template files (see raylib documentation)
4. Build with `-DANDROID=ON`

## License

MIT License - feel free to use, modify, and distribute.

---

*"The Star Void was created by an ancient civilization."*