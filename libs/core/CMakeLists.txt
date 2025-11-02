cmake_minimum_required(VERSION 3.28)

project(Core)

add_library(Core STATIC)

target_sources(Core
    PUBLIC
    src/ClickListener.cpp
    src/HoverListener.cpp
    src/Random.cpp
)

# Set C++ standard for the target
target_compile_features(Core PRIVATE cxx_std_23)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Core PRIVATE
    sfml-system)
