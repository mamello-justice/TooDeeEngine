cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Force MSVC to use dynamic runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

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
    if(DEFINED ENV{${var}})
        set(${var} "$ENV{${var}}")
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

project(TooDeeEngine
    VERSION 1.0.0
    LANGUAGES CXX
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# ===== FIND PACKAGES =====

find_package(SFML COMPONENTS System Window Graphics CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-inih CONFIG REQUIRED)

# Option to build shared libraries (OFF = static by default)
option(TOO_DEE_ENGINE_BUILD_SHARED "Build TooDeeCore and TooDeeEngine as shared libraries" OFF)

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

# ===== INSTALL & PACKAGE CONFIG =====

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    EXPORT TooDeeTargets
    FILE TooDeeTargets.cmake
    NAMESPACE TooDeeEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TooDeeEngine
    COMPONENT engine
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/TooDeeEngineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TooDeeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/TooDeeEngineConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TooDeeEngine
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/TooDeeEngineConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/TooDeeEngineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TooDeeEngine
    COMPONENT engine
)

# ===== PACKAGING (CPack) =====

set(CPACK_PACKAGE_NAME "TooDeeEngine")
set(CPACK_PACKAGE_VENDOR "TooDeeEngine")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TooDeeEngine 2D Game Engine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/mamello-justice/TooDeeEngine")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist")

# ===== ARTIFACT NAMING =====
# Per-platform archives are named:
#   too-dee-engine-<version>-<os>-<arch>-<linkage>.<ext>
# and the all-in-one universal package is named:
#   too-dee-engine-<version>-all-universal.nupkg

set(TOO_DEE_ENGINE_PACKAGE_ID "too-dee-engine")

if(WIN32)
    set(TOO_DEE_ENGINE_OS_ID "windows")
elseif(APPLE)
    set(TOO_DEE_ENGINE_OS_ID "macos")
else()
    set(TOO_DEE_ENGINE_OS_ID "linux")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" TOO_DEE_ENGINE_SYSTEM_PROCESSOR)
if(TOO_DEE_ENGINE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
    set(TOO_DEE_ENGINE_ARCH_ID "x64")
elseif(TOO_DEE_ENGINE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(TOO_DEE_ENGINE_ARCH_ID "arm64")
else()
    set(TOO_DEE_ENGINE_ARCH_ID "${TOO_DEE_ENGINE_SYSTEM_PROCESSOR}")
endif()

if(TOO_DEE_ENGINE_BUILD_SHARED)
    set(TOO_DEE_ENGINE_LINKAGE_ID "shared")
else()
    set(TOO_DEE_ENGINE_LINKAGE_ID "static")
endif()

set(TOO_DEE_ENGINE_ARTIFACT_NAME
    "${TOO_DEE_ENGINE_PACKAGE_ID}-${PROJECT_VERSION}-${TOO_DEE_ENGINE_OS_ID}-${TOO_DEE_ENGINE_ARCH_ID}-${TOO_DEE_ENGINE_LINKAGE_ID}")
set(TOO_DEE_ENGINE_UNIVERSAL_ARTIFACT_NAME
    "${TOO_DEE_ENGINE_PACKAGE_ID}-${PROJECT_VERSION}-all-universal")

# Consumed by cmake/packaging/PackUniversal.cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/TooDeePackageInfo.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/TooDeePackageInfo.cmake"
    @ONLY
)

# Every generator below derives the artifact name from
# TOO_DEE_ENGINE_ARTIFACT_NAME
set(CPACK_PACKAGE_FILE_NAME "${TOO_DEE_ENGINE_ARTIFACT_NAME}")

# Per-component packaging
set(CPACK_COMPONENTS_ALL engine editor cli)
set(CPACK_COMPONENT_ENGINE_DISPLAY_NAME "TooDeeEngine Library")
set(CPACK_COMPONENT_ENGINE_DESCRIPTION "Static/Shared libraries and headers")
set(CPACK_COMPONENT_EDITOR_DISPLAY_NAME "TooDeeEditor")
set(CPACK_COMPONENT_EDITOR_DESCRIPTION "ImGui-based game editor")
set(CPACK_COMPONENT_CLI_DISPLAY_NAME "tde CLI")
set(CPACK_COMPONENT_CLI_DESCRIPTION "Command-line interface tool")

# Windows: portable ZIP + MSI via WiX Toolset
if(WIN32)
    set(CPACK_GENERATOR "ZIP;WIX")
    set(CPACK_WIX_UPGRADE_GUID "A5D71F3E-67C1-4F20-B8E8-93F5B8C7E2D1")
    set(CPACK_WIX_PRODUCT_GUID "C8F7A6B5-E4D3-4C2B-A190-87654321FEDC")
    set(CPACK_WIX_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_WIX_ARCHITECTURE "${TOO_DEE_ENGINE_ARCH_ID}")
    set(CPACK_WIX_INCLUDE_EXPRESS_CODE "ON")
endif()

# Linux: portable TGZ + DEB
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsfml-system3, libsfml-window3, libsfml-graphics3")
endif()

# macOS: portable TGZ + DMG
if(APPLE)
    set(CPACK_GENERATOR "TGZ;DragNDrop")
endif()

include(CPack)
