# ShootEm Example

A space shooter game demonstrating action-based game mechanics with TooDeeEngine.

## Overview

This example showcases:
- Player spaceship control
- Bullet firing and projectile management
- Enemy waves and spawning
- Health and damage systems
- Score tracking
- Game state transitions

## Running the Example

```bash
just example ShootEm
```

## What You'll Learn

- Player input for continuous movement (not grid-based)
- Projectile management and physics
- Enemy spawn waves and patterns
- Collision-based damage system
- Health/lives management
- Game over and restart mechanics
- Score display and tracking

## Source Files

- `include/` - Header files for entities and scenes
- `Scenes/` - Scene implementations
- `Shared/` - Shared utilities and constants
- `src/` - Main implementation files

## Features

- **Player Controls**
  - Smooth spaceship movement
  - Multi-directional shooting
  - Health/Shield system

- **Enemies**
  - Multiple enemy types
  - Wave-based spawning
  - AI attack patterns

- **Combat System**
  - Bullet physics
  - Collision detection and damage
  - Explosion effects

- **Game Management**
  - Lives/Health tracking
  - Score system
  - Game over and restart states

## Game Controls

- **Arrow Keys** or **WASD** - Move spaceship
- **Space** - Fire bullets
- **ESC** - Quit

## Configuration

Game settings are configured in `bin/shoot_em/config.ini`:
```ini
[window]
width=1200
height=800
title=ShootEm

[game]
fps=60
```

## Next Steps

Explore other examples:
- [HelloWorld](../HelloWorld/README.md) - Basic setup
- [GridExample](../GridExample/README.md) - Grid mechanics
- [MegaMario](../MegaMario/README.md) - Platformer gameplay
