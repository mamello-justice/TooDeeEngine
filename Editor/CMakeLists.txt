cmake_minimum_required(VERSION 3.28)

project(TooDeeEditor LANGUAGES CXX)

# Executable Target
add_executable(TooDeeEditor)

target_compile_features(TooDeeEditor
    PRIVATE
    cxx_std_23
)

target_sources(TooDeeEditor
    PRIVATE
    src/main.cpp
    src/Editor.cpp
)

target_include_directories(TooDeeEditor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeEditor
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    imgui
    ImGui-SFML::ImGui-SFML
    Core
    INIH
    GameEngine
    debug GridExample
    debug HelloWorld
    debug MegaMario_Menu
    debug MegaMario_Play
    debug MovingShapes
    debug ShootEm_Menu
    debug ShootEm_Play
)
