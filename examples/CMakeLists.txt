cmake_minimum_required(VERSION 3.28)

set(TOO_DEE_ENGINE_EXAMPLE_TARGET $ENV{TOO_DEE_ENGINE_EXAMPLE_TARGET})

if(DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET)
    message(STATUS "Building ${TOO_DEE_ENGINE_EXAMPLE_TARGET} Example")
else()
    message(STATUS "Building Examples")
endif()

if((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "hello_world"))
    add_subdirectory(hello_world)
endif()

if((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "moving_shapes"))
    add_subdirectory(moving_shapes)
endif()

if((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "native_scripting"))
    add_subdirectory(native_scripting)
endif()

if(TOO_DEE_ENGINE_QJS_SCRIPTING AND ((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "javascript_scripting")))
    add_subdirectory(javascript_scripting)
endif()

if(TOO_DEE_ENGINE_QJS_SCRIPTING AND ((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "typescript_scripting")))
    add_subdirectory(typescript_scripting)
endif()

if(TOO_DEE_ENGINE_LUA_SCRIPTING AND ((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "lua_scripting")))
    add_subdirectory(lua_scripting)
endif()