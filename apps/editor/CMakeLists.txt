cmake_minimum_required(VERSION 3.28)

project(TooDeeEditor LANGUAGES CXX)

include(GNUInstallDirs)

message(STATUS "Building Editor")

# Find Packages
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

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
    src/ImGuiDirectoryView.cpp
)

target_include_directories(TooDeeEditor
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TooDeeEditor
    PRIVATE
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    imgui::imgui
    ImGui-SFML::ImGui-SFML
    unofficial::inih::inireader
    TooDeeCore
    TooDeeEngine
)

if (TOO_DEE_ENGINE_QJS_SCRIPTING)
    target_compile_definitions(TooDeeEditor PRIVATE TOO_DEE_ENGINE_QJS_SCRIPTING)
    
    find_package(qjs CONFIG REQUIRED)
    target_link_libraries(TooDeeEditor PRIVATE qjs)
endif()

if (TOO_DEE_ENGINE_LUA_SCRIPTING)
    target_compile_definitions(TooDeeEditor PRIVATE TOO_DEE_ENGINE_LUA_SCRIPTING)
    
    find_package(Lua REQUIRED)
    find_package(sol2 CONFIG REQUIRED)
    target_link_libraries(TooDeeEditor PRIVATE sol2::sol2)
endif()

# Conditionally link example targets only if they were built
if(TOO_DEE_ENGINE_BUILD_EXAMPLES)
    target_compile_definitions(TooDeeEditor PRIVATE BUILD_EXAMPLES)
    if(TARGET HelloWorld)
        target_compile_definitions(TooDeeEditor PRIVATE HELLO_WORLD_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE HelloWorld)
    endif()

    if(TARGET MovingShapes)
        target_compile_definitions(TooDeeEditor PRIVATE MOVING_SHAPES_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE MovingShapes)
    endif()

    if(TARGET NativeScripting)
        target_compile_definitions(TooDeeEditor PRIVATE NATIVE_SCRIPTING_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE NativeScripting)
    endif()
    
    if(TARGET JavaScriptScripting)
        target_compile_definitions(TooDeeEditor PRIVATE JAVASCRIPT_SCRIPTING_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE JavaScriptScripting)
    endif()
    
    if(TARGET TypeScriptScripting)
        target_compile_definitions(TooDeeEditor PRIVATE TYPESCRIPT_SCRIPTING_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE TypeScriptScripting)
    endif()
    
    if(TARGET LuaScripting)
        target_compile_definitions(TooDeeEditor PRIVATE LUA_SCRIPTING_EXAMPLE)
        target_link_libraries(TooDeeEditor PRIVATE LuaScripting)
    endif()
endif()

# ============================================================
# Install
# ============================================================

install(
    TARGETS TooDeeEditor
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT editor
)

if(WIN32)
    install(
        FILES $<TARGET_RUNTIME_DLLS:TooDeeEditor>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT editor
    )
endif()

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui.ini
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT editor
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
    DESTINATION ${CMAKE_INSTALL_BINDIR}/assets
    COMPONENT editor
)
