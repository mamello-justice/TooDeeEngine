cmake_minimum_required(VERSION 3.28)

project(spdlog)

# Fetch
include(FetchContent)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG b8944a4bcd478ee03375c9c50dc8d6c741f43f7b # v2.x
)

FetchContent_MakeAvailable(spdlog)