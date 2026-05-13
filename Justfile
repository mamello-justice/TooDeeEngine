set dotenv-load := true
set unstable := true

EDITOR := 'TooDeeEditor'
ARCHITECTURE := 'x64'
BUILD_DIR := 'build'
CMAKE_GENERATOR := 'Visual Studio 18 2026'

target_name(target) := if target == "hello_world" { "HelloWorldApp" }\
    else if target == "moving_shapes" { "MovingShapesApp" }\
    else if target == "native_scripting" { "NativeScriptingApp" }\
    else if target == "javascript_scripting" { "JavaScriptScriptingApp" }\
    else if target == "typescript_scripting" { "TypeScriptScriptingApp" }\
    else if target == "lua_scripting" { "LuaScriptingApp" }\
    else { target }

BUILD_EXAMPLES := env('TOO_DEE_ENGINE_BUILD_EXAMPLES')

default:
    just --list

setup *$TOO_DEE_ENGINE_EXAMPLE_TARGET:
    cmake -B {{ BUILD_DIR }} -G "{{ CMAKE_GENERATOR }}" -A {{ ARCHITECTURE }}

clean:
    cmake --build {{ BUILD_DIR }} --target clean

build-js:
    pnpm nx run-many --target=build --parallel --all

build:
    cmake --build {{ BUILD_DIR }} --config Debug

build-release:
    cmake --build {{ BUILD_DIR }} --config Release

package:
    dotnet build -c Release

example target: (setup target) build
    cd examples/{{ target }} && ../../build/examples/{{ target }}/Debug/{{ target_name(target) }} ./config.ini

[working-directory('apps/editor')]
edit: build
    ../../build/apps/editor/Debug/TooDeeEditor
