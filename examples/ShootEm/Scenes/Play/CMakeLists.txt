cmake_minimum_required(VERSION 3.28)

project(ShootEm_Play LANGUAGES CXX)

# Library Target
add_library(ShootEm_Play)

target_compile_features(ShootEm_Play
    PUBLIC
    cxx_std_23
)

target_sources(ShootEm_Play
    PUBLIC
    ./GameConfig.cpp
    ./ShootEm_Play.cpp
)

target_include_directories(ShootEm_Play
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ShootEm_Play
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    Core
    INIH
    GameEngine
)
