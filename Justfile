set dotenv-load := true
set unstable := true

ARCHITECTURE := 'x64'
BUILD_DIR := 'build'

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

# Package editor application (WiX MSI on Windows, TGZ/DEB on Linux, DMG/TGZ on macOS)
package-editor: build-release
    cpack --config {{ BUILD_DIR }}/CPackConfig.cmake -C Release -G all -D CPACK_COMPONENTS_ALL=editor -D CPACK_PACKAGE_FILE_NAME=TooDeeEditor-1.0.0-{{ ARCHITECTURE }}

# Build & package editor via WiX toolset (candle.exe + light.exe)
wix-editor: build-release
    cmake -E make_directory dist/wix
    candle.exe -arch x64 -dProductVersion=1.0.0 -dSrcEditorExe={{ BUILD_DIR }}/apps/editor/Release/TooDeeEditor.exe -dEditorSourceDir=apps/editor/ -dRuntimeDir={{ BUILD_DIR }}/apps/editor/Release/ apps/editor/packaging/TooDeeEditor.wxs -out dist/wix/editor.wixobj
    light.exe -out dist/TooDeeEditor-1.0.0.msi dist/wix/editor.wixobj

# Package CLI tool (WiX MSI on Windows, TGZ/DEB on Linux, DMG/TGZ on macOS)
package-cli: build-release
    cpack --config {{ BUILD_DIR }}/CPackConfig.cmake -C Release -G all -D CPACK_COMPONENTS_ALL=cli -D CPACK_PACKAGE_FILE_NAME=tde-1.0.0-{{ ARCHITECTURE }}

# Build & package CLI via WiX toolset (candle.exe + light.exe)
wix-cli: build-release
    cmake -E make_directory dist/wix
    candle.exe -arch x64 -dProductVersion=1.0.0 -dSrcCliExe={{ BUILD_DIR }}/apps/cli/Release/tde.exe -dRuntimeDir={{ BUILD_DIR }}/apps/cli/Release/ apps/cli/packaging/tde.wxs -out dist/wix/cli.wixobj
    light.exe -out dist/tde-1.0.0.msi dist/wix/cli.wixobj

# Package the engine library (headers + static/shared libs)
package-engine: build-release
    cpack --config {{ BUILD_DIR }}/CPackConfig.cmake -C Release -G all -D CPACK_COMPONENTS_ALL=engine -D CPACK_PACKAGE_FILE_NAME=TooDeeEngine-1.0.0-{{ ARCHITECTURE }}

# Build & package everything
package: package-engine package-editor package-cli

example target: (setup target) build
    cd examples/{{ target }} && ../../build/examples/{{ target }}/Debug/{{ target_name(target) }} ./config.ini

[working-directory('apps/editor')]
edit: build
    ../../build/apps/editor/Debug/TooDeeEditor

run *args: build
    build/apps/runner/Debug/tde {{args}}

test: build
    ctest --test-dir build/tests -C Debug
