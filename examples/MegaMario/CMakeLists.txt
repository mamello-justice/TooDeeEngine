cmake_minimum_required(VERSION 3.28)

project(MegaMario LANGUAGES CXX)

add_subdirectory(MenuScene)
add_subdirectory(PlayScene)

# Executable Target
add_executable(MegaMario)

target_compile_features(MegaMario
    PUBLIC
    cxx_std_23
)

target_sources(MegaMario
    PUBLIC
    src/main.cpp
)

target_include_directories(MegaMario
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MegaMario
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
    MegaMario_Menu
    MegaMario_Play
)
