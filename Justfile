set dotenv-load := true
set unstable := true
set lists

BUILD_DIR := 'build'
DIST_DIR := 'dist'
TDE_EXECUTABLE := if os_family() == "windows" { "build/apps/cli/Debug/tde.exe" } else { "build/apps/cli/tde" }
EDITOR_EXECUTABLE := if os_family() == "windows" { "../../build/apps/editor/Debug/TooDeeEditor.exe" } else { "../../build/apps/editor/TooDeeEditor" }
EXAMPLE_EXECUTABLE_SUFFIX := if os_family() == "windows" { ".exe" } else { "" }

# Portable archive format for the host platform
archive_generator := if os_family() == "windows" { "ZIP" } else { "TGZ" }

target_name(target) := if target == "hello_world" { "HelloWorldApp" }\
    else if target == "moving_shapes" { "MovingShapesApp" }\
    else if target == "native_scripting" { "NativeScriptingApp" }\
    else if target == "javascript_scripting" { "JavaScriptScriptingApp" }\
    else if target == "typescript_scripting" { "TypeScriptScriptingApp" }\
    else if target == "lua_scripting" { "LuaScriptingApp" }\
    else { target }

is_example_target(target) := target == "hello_world" || target == "moving_shapes" || target == "native_scripting" || target == "javascript_scripting" || target == "typescript_scripting" || target == "lua_scripting"

example_scripting_env(target) := if target == "javascript_scripting" { "TOO_DEE_ENGINE_QJS_SCRIPTING=ON" }\
    else if target == "typescript_scripting" { "TOO_DEE_ENGINE_QJS_SCRIPTING=ON" }\
    else if target == "lua_scripting" { "TOO_DEE_ENGINE_LUA_SCRIPTING=ON" }\
    else { "" }

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

example target='':
    {{ if assert(is_example_target(target), "Available example targets: hello_world, moving_shapes, native_scripting, javascript_scripting, typescript_scripting, lua_scripting") { "" } else { "" } }}
    cmake -E env TOO_DEE_ENGINE_BUILD_EXAMPLES=ON TOO_DEE_ENGINE_EXAMPLE_TARGET={{target}} {{example_scripting_env(target)}} cmake -S . -B {{BUILD_DIR}}
    cmake --build {{BUILD_DIR}} --config Debug --target {{target_name(target)}}
    cmake -E chdir examples/{{ target }} ../../build/examples/{{ target }}/Debug/{{ target_name(target) }}{{ EXAMPLE_EXECUTABLE_SUFFIX }} ./config.ini

[working-directory('apps/editor')]
edit:
    cmake -E env TOO_DEE_ENGINE_BUILD_EDITOR=ON TOO_DEE_ENGINE_BUILD_EXAMPLES=ON cmake -S ../.. -B ../../{{BUILD_DIR}}
    cmake --build ../../{{BUILD_DIR}} --config Debug --target TooDeeEditor
    {{ EDITOR_EXECUTABLE }}

run *args:
    cmake -S . -B {{ BUILD_DIR }} -DTOO_DEE_ENGINE_BUILD_CLI=ON
    cmake --build {{ BUILD_DIR }} --config Debug --target tde
    {{ TDE_EXECUTABLE }} {{args}}

test:
    cmake -E env TOO_DEE_ENGINE_ENABLE_TESTING=ON TOO_DEE_ENGINE_BUILD_CLI=OFF TOO_DEE_ENGINE_BUILD_EDITOR=OFF TOO_DEE_ENGINE_BUILD_EXAMPLES=OFF TOO_DEE_ENGINE_QJS_SCRIPTING=OFF TOO_DEE_ENGINE_LUA_SCRIPTING=OFF cmake -S . -B {{BUILD_DIR}}
    cmake --build {{BUILD_DIR}} --config Debug
    ctest --test-dir {{BUILD_DIR}} -C Debug --output-on-failure
