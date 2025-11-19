# MegaMario Example

A full-featured platformer game demonstrating the complete capabilities of TooDeeEngine.

## Overview

This example showcases:
- Player movement and jumping mechanics
- Enemy AI and behavior
- Level progression system
- Collectibles and scoring
- Complete game loop with state management

## Running the Example

```bash
just example MegaMario
```

## What You'll Learn

- Advanced entity management with multiple entity types
- Player input handling and physics
- Enemy AI implementation
- Scene management and level loading
- Collision detection and resolution
- Score and collectible systems
- Game state management

## Source Files

- `include/` - Header files for scenes and game logic
- `Scenes/` - Individual scene implementations
- `src/` - Main implementation files

## Features

- **Player Mechanics**
  - Movement left/right
  - Jumping with gravity physics
  - Walking animations
  
- **Enemy AI**
  - Patrol behavior
  - Player collision detection
  - Dynamic spawning

- **Level System**
  - Multiple levels with progression
  - Collectible items
  - Goal/exit conditions

- **Scoring**
  - Points for collectibles
  - High score tracking
  - Display system

## Game Controls

- **Arrow Keys** or **A/D** - Move left/right
- **Space** - Jump
- **ESC** - Quit

## Level Files

Level data is stored in configuration files in the `bin/MegaMario/` directory:
- `level1.txt` - First level
- `level2.txt` - Second level
- `level3.txt` - Third level

## Next Steps

Explore other examples:
- [HelloWorld](../HelloWorld/README.md) - Basic setup
- [GridExample](../GridExample/README.md) - Grid mechanics
- [ShootEm](../ShootEm/README.md) - Space shooter mechanics
