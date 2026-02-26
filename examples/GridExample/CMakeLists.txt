cmake_minimum_required(VERSION 3.28)

project(GridExample LANGUAGES CXX)

# Library Target
add_library(GridExample)

target_compile_features(GridExample
    PUBLIC
    cxx_std_23
)

target_sources(GridExample
    PUBLIC
    ./GridExample.cpp
)

target_include_directories(GridExample
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(GridExample
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
)
