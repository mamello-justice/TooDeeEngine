cmake_minimum_required(VERSION 3.28)

project(ShootEm LANGUAGES CXX)

# Add executable
add_executable(ShootEm)

# Add source and header files
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

# Set C++ standard for the target
target_compile_features(ShootEm PRIVATE cxx_std_23)

# Add include directories
target_include_directories(ShootEm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/components
    ${CMAKE_CURRENT_SOURCE_DIR}/include/layout
)

# Link libraries
target_link_libraries(ShootEm PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    INIH
    Core
    GameEngine
)
