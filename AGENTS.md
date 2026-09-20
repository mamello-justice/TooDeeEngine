# TooDeeEngine Agents Configuration

This document provides context about the TooDeeEngine project structure for global agents. It can be read by any LLM model or agent to understand this C++ game engine project immediately, without figuring out the file system on their own.

## Supported Languages / Scripting Engines

**Current:**
- **C++23**: Primary language for all core engine code

**Active Embeddings:**
- **JavaScript**: Runtime via quickjs-ng (packages/quickjs, a fork of QuickJS) embedding (docs: https://mintlify.wiki/quickjs-ng/), type definitions in `types/`

**Planning/TODO:**
- **Lua**: Planned scripting system with embedded interpreter API

## Project Structure

```
TooDeeEngine/
├── apps/              # C++ executable applications (managed by CMake)
│   └── editor/       # Built-in ImGui-based game editor application
├── packages/         # C++ library components and engine modules
│   ├── core/         # Core engine functionality
│   ├── engine/       # Main game engine implementation
│   ├── imgui/        # Image-based GUI library
│   ├── imgui-sfml/   # ImGui SFML integration
│   ├── inih/         # INI file parser (configuration support)
│   ├── quickjs/      # quickjs-ng fork of QuickJS (docs: https://mintlify.wiki/quickjs-ng/)
│   ├── sfml/         # Simple and Fast Multimedia Library (graphics)
│   └── spdlog/       # Fast C++ logging library
├── CMakeLists.txt    # Root CMake build configuration
├── Justfile          # Just task runner commands (setup, build, test)
├── examples/         # Example games demonstrating engine features
├── types/            # TypeScript type definitions (for JS embeddings)
└── TODO.md           # Project goals and pending items
```

## Key Technologies

- **CMake**: Native C++ build system for cross-platform compilation
- **Just**: Task runner for shell commands (build, test, pack, setup)
- **SFML**: Graphics, windowing, and input handling library
- **ImGui**: Immediate-mode GUI for editor tooling
**QuickJS-ng** (packages/quickjs): Lightweight JS runtime for scriptable game behaviors (fork of QuickJS, docs: https://mintlify.wiki/quickjs-ng/)
- **spdlog**: High-performance C++ logging framework
- **inih**: Configuration file parsing for games

## Mixed-Language Architecture

**Primary C++ Components:**
- All engine logic, rendering, physics, and ECS are written in C++23
- Game files in `packages/engine/` and editor in `apps/editor/` are C++ source
- Managed entirely by `CMakeLists.txt` (root and subdirectory)

**Language Embeddings & JavaScript:**
- JS embedding via quickjs-ng (packages/quickjs) for runtime scripting (docs: https://mintlify.wiki/quickjs-ng/)
- Mixed language workspace using npm/pnpm for JS dependencies
- Type definitions in `types/` support TypeScript development
- Root `package.json` coordinates JavaScript tooling and build hooks

## Configuration Files

### C++ Build System
- `CMakeLists.txt` - Main CMake configuration with subdirectories
- Apps (`apps/editor/CMakeLists.txt`) - Editor executable setup
- Packages use individual `CMakeLists.txt` in each directory

### Task Runner (Just)
- `Justfile` - Define and run tasks: just build, just test, just package

### Mixed-Language Setup
- `package.json` - JavaScript tooling, dependencies, and build scripts
- `pnpm-workspace.yaml` - Workspace configuration for mixed project
- `typescript/` and `tsconfig.base.json` - TypeScript settings for JS components

## Development Commands

```bash
just build            # Main build command
just test             # Run tests
just package          # Create release package
```

### CMake builds:
```bash
cmake -B build        # Configure with CMake
cmake --build build   # Build the project
```

## Notes for Agents

When working on this project:
1. All engine code is written in C++23 with cross-platform compilation via CMake
2. JavaScript embeddings use quickjs-ng (packages/quickjs), a fork of QuickJS with docs at https://mintlify.wiki/quickjs-ng/ and embedding section at https://mintlify.wiki/quickjs-ng/embedding
3. Configuration files use INI format parsed by inih library
4. Justfile provides convenient build automation (setup, build, test, package)
5. Editor and examples are ImGui-based applications for game development
6. Future roadmap includes Lua scripting, network multiplayer, sound system, and advanced physics
