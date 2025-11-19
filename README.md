# TooDeeEngine 🎮

A lightweight, high-performance 2D game engine built with C++ using **SFML** for rendering and **ImGui** for UI and editor tooling. TooDeeEngine provides a flexible entity-component system, physics simulation, and an integrated editor for rapid game development.

## Features

### Core Engine
- **Entity-Component System (ECS)**: Efficient and flexible architecture for game object management
- **Scene Management**: Easy switching between multiple game scenes
- **Animation System**: Built-in support for sprite animations
- **Physics Engine**: Gravity, collision detection (bounding boxes and circles)
- **Input Handling**: Keyboard and event-driven input system
- **Asset Management**: Centralized loading and management of game assets

### Editor
- **Built-in Editor**: Real-time game editor powered by ImGui
- **Configuration Files**: Support for `.ini` configuration files
- **Font Support**: Flexible font rendering and management

### Examples
- **HelloWorld**: Basic engine setup and rendering
- **GridExample**: Grid-based game mechanics
- **MegaMario**: Full-featured platformer game
- **ShootEm**: Space shooter game

## System Requirements

- **OS**: Windows/Linux/macOS
- **Compiler**: MSVC (Visual Studio 17 2022) or GCC/Clang
- **CMake**: Version 3.28 or higher
- **C++ Standard**: C++23 or higher

## Dependencies

- **SFML**: Simple and Fast Multimedia Library for graphics and windowing
- **ImGui**: Immediate Mode GUI for editor and debugging
- **ImGui-SFML**: SFML backend for ImGui
- **inih**: INI file parser for configuration

## Getting Started

### Prerequisites

Ensure you have CMake 3.28+ and a C++23 compatible compiler installed.

### Building from Source

#### Setup
```bash
just setup
```

#### Debug Build
```bash
just build
```

#### Release Build
```bash
just build-release
```

#### Launch the Editor
```bash
just edit
```

## Architecture

### Entity-Component System

The engine uses a type-safe ECS architecture where entities are containers for components:

```cpp
// Create an entity and add components
Entity entity = entityManager.addEntity("player");
entity.add<CTransform>(100, 100);
entity.add<CShape>(32, 32);
entity.add<CInput>();
entity.add<CAnimation>();
```

### Core Components

- **CTransform**: Position and velocity
- **CShape**: Visual representation (rectangle/circle)
- **CAnimation**: Frame-based animations
- **CInput**: Keyboard input handling
- **CGravity**: Gravity physics
- **CBoundingBox/CBoundingCircle**: Collision detection
- **CState**: Entity state machine
- **CScore**: Score tracking
- **CLifespan**: Entity lifetime management

### Game Loop

The engine operates on a standard game loop:

1. **Input**: Process user input
2. **Update**: Update game logic and physics
3. **Render**: Draw entities and UI
4. **Management**: Handle entity lifecycle

## Examples

TooDeeEngine includes example games demonstrating various engine features and game mechanics:

- **[HelloWorld](examples/HelloWorld/README.md)** - Basic engine setup and sprite rendering
- **[GridExample](examples/GridExample/README.md)** - Grid-based game mechanics and pathfinding
- **[MegaMario](examples/MegaMario/README.md)** - Full platformer with AI, levels, and scoring
- **[ShootEm](examples/ShootEm/README.md)** - Space shooter with combat and wave mechanics

Each example includes a dedicated README with detailed information about features, controls, and learning objectives.

### Running Examples

```bash
just example MegaMario
just example ShootEm
```

## Configuration

Games are configured using INI files located in the `bin/` directory:

```ini
[window]
width=1200
height=800
title=MyGame

[game]
fps=60
```

## Building for Distribution

To create a release package:

```bash
just package
```

## Contributing

Contributions are welcome! Please follow these guidelines:
- Write clear commit messages
- Add tests for new features
- Follow the existing code style
- Update documentation as needed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Copyright © 2025 Mamello Justice

## Getting Help

- Check the `examples/` directory for reference implementations
- Review engine headers in `Engine/include/` for API documentation
- Examine configuration files in `bin/` for setup examples

## Roadmap

- [ ] Network multiplayer support
- [ ] Sound and music system
- [ ] Particle effects
- [ ] Advanced physics (rigid bodies)
- [ ] Scripting support (Lua)
- [ ] Mobile platform support

---

**Start building your 2D games with TooDeeEngine today!** 🚀
