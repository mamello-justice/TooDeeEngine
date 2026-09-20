cmake_minimum_required(VERSION 3.28)

project(TooDeeEngine LANGUAGES CXX)

message(STATUS "Building TooDeeEngine")

# Library Target
add_library(TooDeeEngine STATIC)

target_compile_features(TooDeeEngine
    PRIVATE
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeEngine
    PRIVATE
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
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