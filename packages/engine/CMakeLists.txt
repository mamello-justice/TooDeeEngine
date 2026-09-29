cmake_minimum_required(VERSION 3.28)

project(TooDeeEngine LANGUAGES CXX)

include(GNUInstallDirs)

message(STATUS "Building TooDeeEngine")

# Option to build as shared or static library
option(TOO_DEE_ENGINE_BUILD_SHARED "Build TooDeeEngine as a shared library" OFF)

if(TOO_DEE_ENGINE_BUILD_SHARED)
    add_library(TooDeeEngine SHARED)
    set_target_properties(TooDeeEngine PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
else()
    add_library(TooDeeEngine STATIC)
endif()

add_library(TooDeeEngine::TooDeeEngine ALIAS TooDeeEngine)

target_compile_features(TooDeeEngine
    PUBLIC
    cxx_std_23
)

target_sources(TooDeeEngine
    PRIVATE
    src/Action.cpp
    src/Animation.cpp
    src/Assets.cpp
    src/Components.cpp
    src/Entity.cpp
    src/EntityManager.cpp
    src/GameEngine.cpp
    src/Physics.cpp
    src/Renderer.cpp
    src/Scene.cpp
    src/Script.cpp
)

target_include_directories(TooDeeEngine
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/TooDeeEngine>
)

target_link_libraries(TooDeeEngine
    PUBLIC
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
    PRIVATE
    spdlog::spdlog
    unofficial::inih::inireader
)

if (TOO_DEE_ENGINE_QJS_SCRIPTING)
    message(STATUS "JavaScript Scripting Enabled")
    target_compile_definitions(TooDeeEngine PRIVATE TOO_DEE_ENGINE_QJS_SCRIPTING)

    find_package(qjs CONFIG REQUIRED)
    target_link_libraries(TooDeeEngine PRIVATE qjs)
    target_sources(TooDeeEngine PRIVATE src/qjs.cpp)
endif()

if (TOO_DEE_ENGINE_LUA_SCRIPTING)
    message(STATUS "Lua Scripting Enabled")
    target_compile_definitions(TooDeeEngine PRIVATE TOO_DEE_ENGINE_LUA_SCRIPTING)

    find_package(Lua REQUIRED)
    find_package(sol2 CONFIG REQUIRED)
    target_link_libraries(TooDeeEngine PRIVATE sol2::sol2)
endif()

# ============================================================
# Install
# ============================================================

install(
    TARGETS TooDeeEngine
    EXPORT TooDeeTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT engine
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TooDeeEngine
    COMPONENT engine
    FILES_MATCHING PATTERN "*.hpp"
)
