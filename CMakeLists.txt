cmake_minimum_required(VERSION 3.28)

project(TooDeeEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Force MSVC to use dynamic runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Read environment variables set by justfile
if(DEFINED ENV{GIT_PROGRESS})
    set(GIT_PROGRESS $ENV{GIT_PROGRESS})
else()
    set(GIT_PROGRESS false)
endif()

if(DEFINED ENV{TOO_DEE_ENGINE_BUILD_EDITOR})
    set(TOO_DEE_ENGINE_BUILD_EDITOR $ENV{TOO_DEE_ENGINE_BUILD_EDITOR})
else()
    set(TOO_DEE_ENGINE_BUILD_EDITOR false)
endif()

if(DEFINED ENV{TOO_DEE_ENGINE_BUILD_EXAMPLES})
    set(TOO_DEE_ENGINE_BUILD_EXAMPLES $ENV{TOO_DEE_ENGINE_BUILD_EXAMPLES})
else()
    set(TOO_DEE_ENGINE_BUILD_EXAMPLES false)
endif()

add_subdirectory(libs)
add_subdirectory(Engine)

if(TOO_DEE_ENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Conditionally add subdirectories or targets
if(TOO_DEE_ENGINE_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()
