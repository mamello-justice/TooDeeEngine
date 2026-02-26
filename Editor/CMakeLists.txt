cmake_minimum_required(VERSION 3.28)

project(TooDeeEditor LANGUAGES CXX)

message(STATUS "Building Editor")

# Executable Target
add_executable(TooDeeEditor)

target_compile_features(TooDeeEditor
    PRIVATE
    cxx_std_23
)

target_sources(TooDeeEditor
    PRIVATE
    src/main.cpp
    src/Editor.cpp
)

target_include_directories(TooDeeEditor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeEditor
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    imgui
    ImGui-SFML::ImGui-SFML
    INIH
    TooDeeCore
    TooDeeEngine
)

# Conditionally link example targets only if they were built
if(TOO_DEE_ENGINE_BUILD_EXAMPLES)
    target_compile_definitions(TooDeeEditor PRIVATE BUILD_EXAMPLES)
    if(TARGET GridExample)
        target_link_libraries(TooDeeEditor PRIVATE debug GridExample)
    endif()
    if(TARGET HelloWorld)
        target_link_libraries(TooDeeEditor PRIVATE debug HelloWorld)
    endif()
    if(TARGET MegaMario_Menu)
        target_link_libraries(TooDeeEditor PRIVATE debug MegaMario_Menu)
    endif()
    if(TARGET MegaMario_Play)
        target_link_libraries(TooDeeEditor PRIVATE debug MegaMario_Play)
    endif()
    if(TARGET MovingShapes)
        target_link_libraries(TooDeeEditor PRIVATE debug MovingShapes)
    endif()
    if(TARGET ShootEm_Menu)
        target_link_libraries(TooDeeEditor PRIVATE debug ShootEm_Menu)
    endif()
    if(TARGET ShootEm_Play)
        target_link_libraries(TooDeeEditor PRIVATE debug ShootEm_Play)
    endif()
endif()
