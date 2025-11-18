cmake_minimum_required(VERSION 3.28)

project(HelloWorld LANGUAGES CXX)

# Library Target
add_library(HelloWorld)

target_compile_features(HelloWorld
    PUBLIC
    cxx_std_23
)

target_sources(HelloWorld
    PUBLIC
    ./HelloWorld.cpp
)

target_include_directories(HelloWorld
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HelloWorld
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
)
