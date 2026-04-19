cmake_minimum_required(VERSION 3.28)

project(TypeScriptScripting LANGUAGES CXX)

# Library Target
add_library(TypeScriptScripting)

target_compile_features(TypeScriptScripting
    PUBLIC
    cxx_std_23
)

target_sources(TypeScriptScripting
    PUBLIC
    src/TypeScriptScripting.cpp
)

target_include_directories(TypeScriptScripting
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(TypeScriptScripting
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
    INIH
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "typescript_scripting")
    # Executable Target
    add_executable(TypeScriptScriptingApp)

    target_compile_features(TypeScriptScriptingApp
        PUBLIC
        cxx_std_23
    )

    target_sources(TypeScriptScriptingApp
        PUBLIC
        src/main.cpp
    )

    target_include_directories(TypeScriptScriptingApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(TypeScriptScriptingApp
        PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        TooDeeCore
        TooDeeEngine
        TypeScriptScripting
    )
endif()
