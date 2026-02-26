cmake_minimum_required(VERSION 3.28)

project(ShootEm LANGUAGES CXX)

add_subdirectory(Shared)
add_subdirectory(Scenes/Menu)
add_subdirectory(Scenes/Play)

# Executable Target
add_executable(ShootEm)

target_compile_features(ShootEm
    PRIVATE
    cxx_std_23
)

target_sources(ShootEm
    PRIVATE
    src/main.cpp
    src/ShootEm.cpp
)

target_include_directories(ShootEm
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ShootEm
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    INIH
    TooDeeCore
    TooDeeEngine
    ShootEm_Menu
    ShootEm_Play
)
