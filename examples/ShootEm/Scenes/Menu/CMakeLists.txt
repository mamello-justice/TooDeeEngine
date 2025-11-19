cmake_minimum_required(VERSION 3.28)

project(ShootEm_Menu LANGUAGES CXX)

# Library Target
add_library(ShootEm_Menu)

target_compile_features(ShootEm_Menu
    PUBLIC
    cxx_std_23
)

target_sources(ShootEm_Menu
    PUBLIC
    ./ShootEm_Menu.cpp
)

target_include_directories(ShootEm_Menu
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShootEm_Menu
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
    ShootEm_Shared
)
