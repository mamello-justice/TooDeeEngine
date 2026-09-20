cmake_minimum_required(VERSION 3.28)

project(tde LANGUAGES CXX)

message(STATUS "Building Editor")

# Executable Target
add_executable(tde)

target_compile_features(tde
    PRIVATE
    cxx_std_23
)

target_sources(tde
    PRIVATE
    src/main.cpp
)

target_include_directories(tde
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tde
    PRIVATE
    spdlog::spdlog
    sfml-system
    sfml-window
    sfml-graphics
    imgui
    ImGui-SFML::ImGui-SFML
    INIH
    TooDeeCore
    TooDeeEngine
)