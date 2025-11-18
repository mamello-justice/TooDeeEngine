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
    src/Components/Button.cpp
    src/GameConfig.cpp
    src/Layout/Flex.cpp
    src/ShootEm.cpp
    src/ShootEm_Menu.cpp
    src/ShootEm_Play.cpp
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
    Core
    GameEngine
)

# Menu Library Target
add_library(ShootEm_Menu)

target_compile_features(ShootEm_Menu
    PUBLIC
    cxx_std_23
)

target_sources(ShootEm_Menu
    PUBLIC
    src/ShootEm_Menu.cpp
    src/Components/Button.cpp
    src/Layout/Flex.cpp
)

target_include_directories(ShootEm_Menu
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ShootEm_Menu
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
)

# Play Library Target
add_library(ShootEm_Play)

target_compile_features(ShootEm_Play
    PUBLIC
    cxx_std_23
)

target_sources(ShootEm_Play
    PUBLIC
    src/ShootEm_Play.cpp
    src/GameConfig.cpp
)

target_include_directories(ShootEm_Play
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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
