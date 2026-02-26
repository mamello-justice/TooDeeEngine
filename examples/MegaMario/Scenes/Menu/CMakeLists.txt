cmake_minimum_required(VERSION 3.28)

project(MegaMario_Menu LANGUAGES CXX)

# Library Target
add_library(MegaMario_Menu)

target_compile_features(MegaMario_Menu
    PUBLIC
    cxx_std_23
)

target_sources(MegaMario_Menu
    PUBLIC
    ./MegaMario_Menu.cpp
)

target_include_directories(MegaMario_Menu
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MegaMario_Menu
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
)
