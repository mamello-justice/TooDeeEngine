cmake_minimum_required(VERSION 3.28)

project(ImGui)

add_library(ImGui STATIC)

target_sources(ImGui
    PRIVATE
    src/imgui.cpp
    src/imgui_demo.cpp
    src/imgui_draw.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui_stdlib.cpp
    src/imgui-SFML.cpp
)

target_include_directories(ImGui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ImGui PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
)
