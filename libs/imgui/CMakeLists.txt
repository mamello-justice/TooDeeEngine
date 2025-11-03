cmake_minimum_required(VERSION 3.28)

project(imgui)

# Options
set(IMGUI_DIR ${imgui_SOURCE_DIR} PARENT_SCOPE)

# Fetch
include(FetchContent)

FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG 126d004f9e1eef062bf4b044b3b2faaf58d48c51 # 1.91.9-docking
)

FetchContent_MakeAvailable(imgui)

# Library Target
add_library(imgui STATIC)

target_sources(imgui
    PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui
    PUBLIC
    ${imgui_SOURCE_DIR}
)
