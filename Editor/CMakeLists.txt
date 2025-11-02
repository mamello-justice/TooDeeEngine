cmake_minimum_required(VERSION 3.28)

project(TooDeeEditor LANGUAGES CXX)

add_subdirectory(imgui)

# Add executable
add_executable(TooDeeEditor)

# Add source and header files
target_sources(TooDeeEditor
    PRIVATE
    src/main.cpp
    src/Editor.cpp
)

# Set C++ standard for the target
target_compile_features(TooDeeEditor PRIVATE cxx_std_23)

# Add include directories
target_include_directories(TooDeeEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(TooDeeEditor PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    sfml-network
    ImGui
    INIH
    Core
    GameEngine
)
