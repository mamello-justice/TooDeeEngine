cmake_minimum_required(VERSION 3.28)

set(TOO_DEE_ENGINE_EXAMPLE_TARGET $ENV{TOO_DEE_ENGINE_EXAMPLE_TARGET})

if(DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET)
    message(STATUS "Building ${TOO_DEE_ENGINE_EXAMPLE_TARGET} Example")
else()
    message(STATUS "Building Examples")
endif()

if((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "HelloWorld"))
    add_subdirectory(hello_world)
endif()

if((NOT DEFINED TOO_DEE_ENGINE_EXAMPLE_TARGET) OR (TOO_DEE_ENGINE_EXAMPLE_TARGET STREQUAL "MovingShapes"))
    add_subdirectory(moving_shapes)
endif()
