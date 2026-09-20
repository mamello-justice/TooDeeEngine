cmake_minimum_required(VERSION 3.28)

project(TooDeeEngine LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force MSVC to use dynamic runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# ===== ENV VARIABLES -> NORMAL VARIABLES =====

set(ENV_LIST
    GIT_PROGRESS
    TOO_DEE_ENGINE_BUILD_EDITOR
    TOO_DEE_ENGINE_BUILD_CLI
    TOO_DEE_ENGINE_BUILD_EXAMPLES
    TOO_DEE_ENGINE_QJS_SCRIPTING
    TOO_DEE_ENGINE_LUA_SCRIPTING
    TOO_DEE_ENGINE_ENABLE_TESTING
)

foreach(var ${ENV_LIST})
    # Copy value if the environment variable is defined
    if(DEFINED ENV{${var}})
        set(${var} "$ENV{${var}}")
    else()
        set(${var} false)
    endif()
endforeach()

# ===== ADD SUBDIRECTORIES =====

add_subdirectory(packages)

if(TOO_DEE_ENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(TOO_DEE_ENGINE_BUILD_EDITOR)
    add_subdirectory(apps/editor)
endif()

if(TOO_DEE_ENGINE_BUILD_CLI)
    add_subdirectory(apps/cli)
endif()

if (TOO_DEE_ENGINE_ENABLE_TESTING)
    enable_testing()
    
    add_subdirectory(tests)
endif()
