cmake_minimum_required(VERSION 3.28)

project(imgui-sfml)

# Options
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

# Fetch
include(FetchContent)

FetchContent_Declare(imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 5e8f8fb857c8f3cff1096bf3e4402c64e3bcf6af # 3.0
    GIT_PROGRESS ${GIT_PROGRESS}
)

FetchContent_MakeAvailable(imgui-sfml)
