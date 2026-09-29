---
title: Requirements
description: System requirements for building and running TooDeeEngine across Windows, Linux, and macOS.
---

This page documents the system requirements for building TooDeeEngine from source as well as running the editor and CLI applications.

## Summary

| Requirement | Minimum | Recommended |
|---|---|---|
| **OS** | Windows 10, Ubuntu 22.04, macOS 12 | Windows 11, Ubuntu 24.04, macOS 14 |
| **Compiler** | C++23 compatible | Latest stable C++23 |
| **CMake** | 3.28 | 3.28+ |
| **RAM** | 8 GB | 16 GB+ |
| **Disk Space** | 1 GB | 5 GB+ |

## Windows

### Compiler

- **MSVC**: Visual Studio 2022 17.10+ (with C++23 support)
- **MinGW**: GCC 14+ (MinGW-w64)
- **Clang**: Clang 17+ (via LLVM)

Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with the "Desktop development with C++" workload.

### System Requirements

- **OS**: Windows 10 21H2 or later, Windows 11
- **Architecture**: x64
- **Python**: Python 3.10+ (for vcpkg and build scripts)
- **Git**: Git 2.30+
- **WiX Toolset**: 3.11+ (only required to build MSI packages; `candle.exe` and `light.exe` must be on `PATH`)

### Setup

```bash
# Install Just via Scoop or Chocolatey
scoop install just
# or
choco install just
```

### Editor Runtime Dependencies

The editor requires the [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-redist) for the target machine.

## Linux

### Compiler

- **GCC**: GCC 13+ (g++ 13+)
- **Clang**: Clang 17+ (clang++ 17+)

Install GCC 13+ or Clang 17+:

```bash
# Ubuntu 24.04 / Debian testing
sudo apt install build-essential g++-13

# Fedora 40+
sudo dnf install gcc gcc-c++

# Arch Linux
sudo pacman -S gcc
```

### System Requirements

- **OS**: Ubuntu 22.04+, Fedora 40+, Arch Linux (or equivalent)
- **Architecture**: x86_64, ARM64
- **Display Server**: X11 or Wayland (for editor)
- **Python**: Python 3.10+ (for vcpkg)
- **Git**: Git 2.30+
- **Pkg-config**: For dependency discovery

### Setup

```bash
# Install Just
sudo apt install just      # Debian/Ubuntu
sudo dnf install just      # Fedora
sudo pacman -S just        # Arch Linux

# Install build essentials
sudo apt install build-essential cmake git python3 pkg-config
```

### Editor Runtime Dependencies

On Linux, the editor links dynamically against system libraries. Ensure the following are installed at runtime:

```bash
# Ubuntu/Debian
sudo apt install libgl1 libgl1-mesa-glx libx11-6 libxrandr2 libxcursor1 libxi6

# Fedora
sudo dnf install mesa-libGL libX11 libXrandr libXcursor libXi
```

## macOS

### Compiler

- **Apple Clang**: Bundled with Xcode Command Line Tools (Clang 17+)
- **Homebrew GCC**: GCC 14+ (GCC 14+ via Homebrew)

Install Xcode Command Line Tools:

```bash
xcode-select --install
```

### System Requirements

- **OS**: macOS 12 (Monterey) or later
- **Architecture**: Apple Silicon (arm64) or Intel (x86_64)
- **Xcode**: Xcode 15+ (for Command Line Tools)
- **Python**: Python 3.10+ (for vcpkg)
- **Git**: Git 2.30+ (via Xcode Tools or Homebrew)

### Setup

```bash
# Install Homebrew (https://brew.sh)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install just cmake python3 git

# Install Xcode Command Line Tools
xcode-select --install
```

### Editor Runtime Dependencies

The editor bundles all dependencies. No additional system libraries are required beyond the Xcode Command Line Tools.

## Dependency Libraries

TooDeeEngine uses [vcpkg](https://github.com/microsoft/vcpkg) for C++ dependency management. Dependencies are fetched automatically during the CMake configure step:

- **SFML** 3.0 — Graphics, windowing, and input handling
- **spdlog** 1.17 — Logging framework
- **inih** 62 — INI file parser for configuration
- **ImGui** 1.91 (optional, for editor) — Immediate-mode GUI
- **ImGui-SFML** 3.0 (optional, for editor) — ImGui SFML backend
- **QuickJS-ng** 0.15 (optional, for JS scripting) — JavaScript runtime
- **Lua** 5.4 (optional, for Lua scripting) — Lua runtime
- **sol2** 3.5 (optional, for Lua scripting) — Lua bindings
- **GoogleTest** 1.17 (optional, for testing) — Unit testing framework

## Just Task Runner

All builds use [Just](https://github.com/casey/just) as the task runner. If `just` is not available, you can run the equivalent CMake commands directly:

```bash
# Equivalent to `just setup`
cmake -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake

# Equivalent to `just build`
cmake --build build --config Debug

# Equivalent to `just build-release`
cmake --build build --config Release
```
