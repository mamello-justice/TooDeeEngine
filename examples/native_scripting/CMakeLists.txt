cmake_minimum_required(VERSION 3.28)

project(NativeScripting LANGUAGES CXX)

# Library Target
add_library(NativeScripting)

target_compile_features(NativeScripting
    PUBLIC
    cxx_std_23
)

target_sources(NativeScripting
    PUBLIC
    src/NativeScripting.cpp
)

target_include_directories(NativeScripting
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(NativeScripting
    PUBLIC
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
    TooDeeEngine
    unofficial::inih::inireader
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "native_scripting")
    # Executable Target
    add_executable(NativeScriptingApp)

    target_compile_features(NativeScriptingApp
        PUBLIC
        cxx_std_23
    )

    target_sources(NativeScriptingApp
        PUBLIC
        src/main.cpp
    )

    target_include_directories(NativeScriptingApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(NativeScriptingApp
        PUBLIC
        SFML::System
        SFML::Window
        SFML::Graphics
        TooDeeCore
        TooDeeEngine
        NativeScripting
    )
endif()

