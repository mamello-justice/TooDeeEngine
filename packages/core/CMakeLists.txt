cmake_minimum_required(VERSION 3.28)

project(TooDeeCore)

# Library Target
add_library(TooDeeCore STATIC)

target_sources(TooDeeCore
    PUBLIC
    src/ClickListener.cpp
    src/HoverListener.cpp
    src/Random.cpp
)

target_compile_features(TooDeeCore
    PRIVATE
    cxx_std_23)

target_include_directories(TooDeeCore
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeCore
    PRIVATE
    sfml-system)
