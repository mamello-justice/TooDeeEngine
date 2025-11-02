cmake_minimum_required(VERSION 3.28)

project(MegaMario LANGUAGES CXX)

# Add executable
add_executable(MegaMario)

# Add source and header files
target_sources(MegaMario
    PRIVATE
    src/main.cpp
    src/Scene_Menu.cpp
    src/Scene_Play.cpp
)

# Set C++ standard for the target
target_compile_features(MegaMario PRIVATE cxx_std_23)

# Add include directories
target_include_directories(MegaMario
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link SFML libraries
target_link_libraries(MegaMario PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    Core
    GameEngine
)
