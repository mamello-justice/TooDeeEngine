set dotenv-load := true
set unstable := true

BUILD_DIR := 'build'
DIST_DIR := 'dist'

# Portable archive format for the host platform
archive_generator := if os_family() == "windows" { "ZIP" } else { "TGZ" }

target_name(target) := if target == "hello_world" { "HelloWorldApp" }\
    else if target == "moving_shapes" { "MovingShapesApp" }\
    else if target == "native_scripting" { "NativeScriptingApp" }\
    else if target == "javascript_scripting" { "JavaScriptScriptingApp" }\
    else if target == "typescript_scripting" { "TypeScriptScriptingApp" }\
    else if target == "lua_scripting" { "LuaScriptingApp" }\
    else { target }

default:
    just --list

# Configure CMake
setup *$TOO_DEE_ENGINE_EXAMPLE_TARGET:
    cmake -B {{ BUILD_DIR }}

# Configure CMake for shared library build
setup-shared:
    cmake -B {{ BUILD_DIR }} -DCMAKE_BUILD_TYPE=Release -DTOO_DEE_ENGINE_BUILD_SHARED=ON

# Remove build artifacts
clean:
    cmake --build {{ BUILD_DIR }} --target clean || true
    pnpm nx reset
    rm -rf tmp build dist

# Remove build artifacts and node_modules
clean-all: clean
    rm -rf node_modules

# Build nx projects
build-js:
    pnpm nx run-many --target=build --parallel --all

# Build CMake projects
build:
    cmake --build {{ BUILD_DIR }} --config Debug

# Build CMake projects in release mode
build-release:
    cmake --build {{ BUILD_DIR }} --config Release

# Build in release mode with shared libraries
build-shared: setup-shared
    cmake --build {{ BUILD_DIR }} --config Release

# Install the engine (headers + libraries) to CMAKE_INSTALL_PREFIX
install: build-release
    cmake --install {{ BUILD_DIR }} --config Release

# Install with shared libraries
install-shared: build-shared
    cmake --install {{ BUILD_DIR }} --config Release

# Package the host platform (portable archive + native installer when the platform provides one)
package-platform: build-release
    cpack --config {{ BUILD_DIR }}/CPackConfig.cmake -C Release

# Package only the portable archive for the host platform
package-archive: build-release
    cpack --config {{ BUILD_DIR }}/CPackConfig.cmake -C Release -G {{ archive_generator }}

# Assemble the all-in-one universal package from the per-platform archives in dist/
package-universal: setup
    cmake -DPACKAGE_INFO_FILE={{ BUILD_DIR }}/TooDeePackageInfo.cmake -DDIST_DIR={{ DIST_DIR }} -P cmake/packaging/PackUniversal.cmake

# Build & package everything
package: package-platform package-universal

example target: (setup target) build
    cd examples/{{ target }} && ../../build/examples/{{ target }}/Debug/{{ target_name(target) }} ./config.ini

[working-directory('apps/editor')]
edit: build
    ../../build/apps/editor/Debug/TooDeeEditor

run *args: build
    build/apps/runner/Debug/tde {{args}}

test: build
    ctest --test-dir build/tests -C Debug
