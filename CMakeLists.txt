cmake_minimum_required(VERSION 3.28)

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

# ===== SETUP vcpkg ======

include(FetchContent)

FetchContent_Declare(vcpkg
    GIT_REPOSITORY https://github.com/microsoft/vcpkg
    GIT_TAG 9e593bb18ea69cc5095e012465dcd675a822ed0d # 2026.07.29
)

FetchContent_MakeAvailable(vcpkg)

# Run the platform-specific Bootstrap Script ---
if(WIN32)
    set(BOOTSTRAP_SCRIPT "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.bat")
else()
    set(BOOTSTRAP_SCRIPT "${vcpkg_SOURCE_DIR}/bootstrap-vcpkg.sh")
endif()

if(NOT EXISTS "${vcpkg_SOURCE_DIR}/vcpkg${CMAKE_EXECUTABLE_SUFFIX}")
    message(STATUS "Bootstrapping vcpkg...")
    execute_process(
        COMMAND ${BOOTSTRAP_SCRIPT} -disableMetrics
        WORKING_DIRECTORY ${vcpkg_SOURCE_DIR}
        RESULT_VARIABLE BOOTSTRAP_RESULT
    )
    if(NOT BOOTSTRAP_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to bootstrap vcpkg")
    endif()
endif()

# Conditional features
if (TOO_DEE_ENGINE_BUILD_EDITOR)
    list(APPEND VCPKG_MANIFEST_FEATURES "gui")
endif()

if (TOO_DEE_ENGINE_QJS_SCRIPTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "qjs")
endif()

if (TOO_DEE_ENGINE_LUA_SCRIPTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "lua")
endif()

if (TOO_DEE_ENGINE_ENABLE_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

# Set the Toolchain File BEFORE project() ---
set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")

project(TooDeeEngine LANGUAGES CXX)

# ===== FIND PACKAGES =====

find_package(SFML COMPONENTS System Window Graphics CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-inih CONFIG REQUIRED)


# ===== ADD SUBDIRECTORIES =====

add_subdirectory(packages/core)
add_subdirectory(packages/engine)

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
