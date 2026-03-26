cmake_minimum_required(VERSION 3.28)

project(INIH)

# Fetch
include(FetchContent)

FetchContent_Declare(INIH
    GIT_REPOSITORY https://github.com/benhoyt/inih.git
    GIT_TAG 26254ee9de7681f8825433415443e7116ff24b98 # v62
    GIT_PROGRESS ${GIT_PROGRESS}
)

FetchContent_MakeAvailable(INIH)

# Library Target
add_library(INIH STATIC)

target_sources(INIH
    PRIVATE
    ${inih_SOURCE_DIR}/ini.c
    ${inih_SOURCE_DIR}/cpp/INIReader.cpp
    PUBLIC
    ${inih_SOURCE_DIR}/ini.h
    ${inih_SOURCE_DIR}/cpp/INIReader.h
)

target_include_directories(INIH
    PUBLIC
    ${inih_SOURCE_DIR}
    ${inih_SOURCE_DIR}/cpp
)
