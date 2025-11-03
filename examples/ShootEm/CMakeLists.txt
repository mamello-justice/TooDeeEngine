cmake_minimum_required(VERSION 3.28)

project(ShootEm LANGUAGES CXX)

# Executable Target
add_executable(ShootEm)

target_compile_features(ShootEm
    PRIVATE
    cxx_std_23
)

target_sources(ShootEm
    PRIVATE
    src/main.cpp
    src/components/Button.cpp
    src/layout/Flex.cpp
    src/GameConfig.cpp
    src/ShootEm.cpp
    src/Scene_Menu.cpp
    src/Scene_Play.cpp
)

target_include_directories(ShootEm
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/components
    ${CMAKE_CURRENT_SOURCE_DIR}/include/layout
)

target_link_libraries(ShootEm
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    INIH
    Core
    GameEngine
)
