set dotenv-load := true
set unstable := true

EDITOR := 'TooDeeEditor'
ARCHITECTURE := 'x64'
BUILD_DIR := 'build'
CMAKE_GENERATOR := 'Visual Studio 17 2022'

target_name(target) := if target == "hello_world" { "HelloWorld" } else if target == "moving_shapes" { "MovingShapes" } else if target == "typescript_scripting" { "TypeScriptScripting" } else { target }

BUILD_EXAMPLES := env('TOO_DEE_ENGINE_BUILD_EXAMPLES')

default:
    just --list

setup *$TOO_DEE_ENGINE_EXAMPLE_TARGET:
    cmake -B {{ BUILD_DIR }} -G "{{ CMAKE_GENERATOR }}" -A {{ ARCHITECTURE }}

clean:
    cmake --build {{ BUILD_DIR }} --target clean

build:
    cmake --build {{ BUILD_DIR }} --config Debug
    @if [ "{{ BUILD_EXAMPLES }}" = "true" ]; then nx build @too-dee-engine/typescript-scripting-example ; fi

build-release:
    cmake --build {{ BUILD_DIR }} --config Release

package:
    dotnet build -c Release

[working-directory('runtime')]
example target: (setup target) build
    ../build/examples/{{ target }}/Debug/{{ target_name(target) }}Executable ./{{ target }}/config.ini

[working-directory('runtime')]
edit: build
    ../build/apps/editor/Debug/{{ EDITOR }} ./editor/config.ini 
