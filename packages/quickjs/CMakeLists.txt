cmake_minimum_required(VERSION 3.28)

project(quickjs)

include(FetchContent)

FetchContent_Declare(quickjs
    GIT_REPOSITORY https://github.com/quickjs-ng/quickjs.git
    GIT_TAG dec012362bd93876449f3ecff4f835b2eba89bab # v0.13.0
)
FetchContent_MakeAvailable(quickjs)
