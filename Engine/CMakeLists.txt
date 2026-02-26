cmake_minimum_required(VERSION 3.28)

message(STATUS "Building TooDeeEngine")

project(TooDeeEngine)

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
    src/Scene.cpp
)

target_include_directories(TooDeeEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeEngine
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    INIH
)
