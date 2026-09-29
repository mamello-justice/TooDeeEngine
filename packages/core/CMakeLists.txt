cmake_minimum_required(VERSION 3.28)

project(TooDeeCore)

include(GNUInstallDirs)

# Option to build as shared or static library
option(TOO_DEE_ENGINE_BUILD_SHARED "Build TooDeeCore as a shared library" OFF)

if(TOO_DEE_ENGINE_BUILD_SHARED)
    add_library(TooDeeCore SHARED)
    set_target_properties(TooDeeCore PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
else()
    add_library(TooDeeCore STATIC)
endif()

add_library(TooDeeCore::TooDeeCore ALIAS TooDeeCore)

target_sources(TooDeeCore
    PRIVATE
    src/ClickListener.cpp
    src/Command.cpp
    src/HoverListener.cpp
    src/Random.cpp
    src/Utils.cpp
)

target_compile_features(TooDeeCore
    PUBLIC
    cxx_std_23
)

target_include_directories(TooDeeCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/TooDeeEngine>
)

target_link_libraries(TooDeeCore
    PUBLIC
    SFML::Graphics
    PRIVATE
    spdlog::spdlog
)

# ============================================================
# Install
# ============================================================

install(
    TARGETS TooDeeCore
    EXPORT TooDeeTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT engine
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/TooDeeEngine
    COMPONENT engine
)
