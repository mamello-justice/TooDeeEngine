cmake_minimum_required(VERSION 3.28)

project(MegaMario LANGUAGES CXX)

# Executable Target
add_executable(MegaMario)

target_compile_features(MegaMario
    PRIVATE
    cxx_std_23
)

target_sources(MegaMario
    PRIVATE
    src/main.cpp
    src/Scene_Menu.cpp
    src/Scene_Play.cpp
)

target_include_directories(MegaMario
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MegaMario
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
)
