cmake_minimum_required(VERSION 3.28)

project(TooDeeEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FetchContent for dependency management
include(FetchContent)

# Configure SFML options
set(SFML_BUILD_NETWORK TRUE)
set(SFML_BUILD_AUDIO TRUE)
set(SFML_BUILD_GRAPHICS TRUE)
set(SFML_BUILD_WINDOW TRUE)
set(SFML_BUILD_EXAMPLES FALSE)
set(SFML_BUILD_TEST_SUITE FALSE)
set(SFML_BUILD_DOC FALSE)
set(SFML_USE_STATIC_STD_LIBS FALSE)
set(SFML_GENERATE_PDB TRUE)
set(SFML_USE_SYSTEM_DEPS FALSE)

# Force MSVC to use dynamic runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# Declare SFML dependency
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2)

# Make SFML available
FetchContent_MakeAvailable(SFML)

add_subdirectory(libs/core)
add_subdirectory(libs/inih)
add_subdirectory(Editor)
add_subdirectory(Engine)
add_subdirectory(examples)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()
