cmake_minimum_required(VERSION 3.28)

project(JavaScriptScripting LANGUAGES CXX)

# Library Target
add_library(JavaScriptScripting)

target_compile_features(JavaScriptScripting
    PUBLIC
    cxx_std_23
)

target_sources(JavaScriptScripting
    PUBLIC
    src/JavaScriptScripting.cpp
)

target_include_directories(JavaScriptScripting
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(JavaScriptScripting
    PUBLIC
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
    TooDeeEngine
    unofficial::inih::inireader
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "javascript_scripting")
    # Executable Target
    add_executable(JavaScriptScriptingApp)

    target_compile_features(JavaScriptScriptingApp
        PUBLIC
        cxx_std_23
    )

    target_sources(JavaScriptScriptingApp
        PUBLIC
        src/main.cpp
    )

    target_include_directories(JavaScriptScriptingApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(JavaScriptScriptingApp
        PUBLIC
        SFML::System
        SFML::Window
        SFML::Graphics
        TooDeeCore
        TooDeeEngine
        JavaScriptScripting
    )
endif()

