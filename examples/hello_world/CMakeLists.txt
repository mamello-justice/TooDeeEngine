cmake_minimum_required(VERSION 3.28)

project(HelloWorld LANGUAGES CXX)

# Library Target
add_library(HelloWorld)

target_compile_features(HelloWorld
    PUBLIC
    cxx_std_23
)

target_sources(HelloWorld
    PUBLIC
    ./HelloWorld.cpp
)

target_include_directories(HelloWorld
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HelloWorld
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "hello_world")
    add_executable(HelloWorldApp)

    target_compile_features(HelloWorldApp
        PUBLIC
        cxx_std_23
    )

    target_sources(HelloWorldApp
        PUBLIC
        ./main.cpp
    )

    target_include_directories(HelloWorldApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(HelloWorldApp
        PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        TooDeeCore
        TooDeeEngine
        HelloWorld
    )
endif()

