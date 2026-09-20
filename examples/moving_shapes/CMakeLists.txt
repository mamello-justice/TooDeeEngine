cmake_minimum_required(VERSION 3.28)

project(MovingShapes LANGUAGES CXX)

# Library Target
add_library(MovingShapes)

target_compile_features(MovingShapes
    PUBLIC
    cxx_std_23
)

target_sources(MovingShapes
    PUBLIC
    src/MovingShapes.cpp
)

target_include_directories(MovingShapes
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(MovingShapes
    PUBLIC
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
    TooDeeEngine
    unofficial::inih::inireader
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "moving_shapes")
    add_executable(MovingShapesApp)

    target_compile_features(MovingShapesApp
        PUBLIC
        cxx_std_23
    )

    target_sources(MovingShapesApp
        PUBLIC
        src/main.cpp
    )

    target_include_directories(MovingShapesApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(MovingShapesApp
        PUBLIC
        SFML::System
        SFML::Window
        SFML::Graphics
        TooDeeCore
        TooDeeEngine
        MovingShapes
    )
endif()

