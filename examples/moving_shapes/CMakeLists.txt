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
    ./MovingShapes.cpp
)

target_include_directories(MovingShapes
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MovingShapes
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
    INIH
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "moving_shapes")
    add_executable(MovingShapesApp)

    target_compile_features(MovingShapesApp
        PUBLIC
        cxx_std_23
    )

    target_sources(MovingShapesApp
        PUBLIC
        ./main.cpp
    )

    target_include_directories(MovingShapesApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(MovingShapesApp
        PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        TooDeeCore
        TooDeeEngine
        MovingShapes
    )
endif()

