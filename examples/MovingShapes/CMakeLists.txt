cmake_minimum_required(VERSION 3.28)

project(MovingShapes LANGUAGES CXX)

# Library Target
add_library(MovingShapes)

target_compile_features(MovingShapes
    PUBLIC
    cxx_std_23
)

target_sources(MovingShapes
    PUBLIC
    ./MovingShapes.cpp
)

target_include_directories(MovingShapes
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MovingShapes
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
    INIH
)
