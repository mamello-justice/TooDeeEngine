cmake_minimum_required(VERSION 3.28)

project(tde LANGUAGES CXX)

include(GNUInstallDirs)

message(STATUS "Building CLI")

# Executable Target
add_executable(tde)

target_compile_features(tde
    PRIVATE
    cxx_std_23
)

target_sources(tde
    PRIVATE
    src/main.cpp
)

target_include_directories(tde
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tde
    PRIVATE
    spdlog::spdlog
    SFML::System
    SFML::Window
    SFML::Graphics
    unofficial::inih::inireader
    TooDeeCore
    TooDeeEngine
)

# ============================================================
# Install
# ============================================================

if(WIN32)
    install(
        FILES $<TARGET_RUNTIME_DLLS:tde>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT cli
    )
endif()

install(
    TARGETS tde
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT cli
)
