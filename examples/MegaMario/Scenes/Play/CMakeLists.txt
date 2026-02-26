cmake_minimum_required(VERSION 3.28)

project(MegaMario_Play LANGUAGES CXX)

# Library Target
add_library(MegaMario_Play)

target_compile_features(MegaMario_Play
    PUBLIC
    cxx_std_23
)

target_sources(MegaMario_Play
    PUBLIC
    ./MegaMario_Play.cpp
)

target_include_directories(MegaMario_Play
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MegaMario_Play
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
)
