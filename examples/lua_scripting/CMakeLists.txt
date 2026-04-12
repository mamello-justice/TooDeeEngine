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
    ./LuaScripting.cpp
)

target_include_directories(LuaScripting
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LuaScripting
    PUBLIC
    sfml-system
    sfml-window
    sfml-graphics
    TooDeeCore
    TooDeeEngine
    INIH
)

if(TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "lua_scripting")
    add_executable(LuaScriptingApp)

    target_compile_features(LuaScriptingApp
        PUBLIC
        cxx_std_23
    )

    target_sources(LuaScriptingApp
        PUBLIC
        ./main.cpp
    )

    target_include_directories(LuaScriptingApp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(LuaScriptingApp
        PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        TooDeeCore
        TooDeeEngine
        LuaScripting
    )
endif()

