cmake_minimum_required(VERSION 3.28)

project(ShootEm_Shared LANGUAGES CXX)

# Library Target
add_library(ShootEm_Shared)

target_compile_features(ShootEm_Shared
    PUBLIC
    cxx_std_23
)

target_sources(ShootEm_Shared
    PUBLIC
    src/Components/Button.cpp
    src/Layout/Flex.cpp
)

target_include_directories(ShootEm_Shared
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ShootEm_Shared
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
)
