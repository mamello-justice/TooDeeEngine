cmake_minimum_required(VERSION 3.28)

project(GameEngine)

# Library Target
add_library(GameEngine STATIC)

target_compile_features(GameEngine
    PRIVATE
    cxx_std_23
)

target_sources(GameEngine
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

target_include_directories(GameEngine
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(GameEngine
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    ImGui-SFML::ImGui-SFML
    Core
    INIH
)
