cmake_minimum_required(VERSION 3.28)

project(LuaScripting LANGUAGES CXX)

# Library Target
add_library(LuaScripting)

target_compile_features(LuaScripting
    PUBLIC
    cxx_std_23
)

target_sources(LuaScripting
    PUBLIC
    src/LuaScripting.cpp
)

target_include_directories(LuaScripting
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(LuaScripting
    PUBLIC
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    TooDeeCore
    TooDeeEngine
    unofficial::inih::inireader
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "lua_scripting")
    add_executable(LuaScriptingApp)

    target_compile_features(LuaScriptingApp
        PUBLIC
        cxx_std_23
    )

    target_sources(LuaScriptingApp
        PUBLIC
        src/main.cpp
    )

    target_include_directories(LuaScriptingApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    target_link_libraries(LuaScriptingApp
        PUBLIC
        SFML::System
        SFML::Window
        SFML::Graphics
        TooDeeCore
        TooDeeEngine
        LuaScripting
    )
endif()

