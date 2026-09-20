cmake_minimum_required(VERSION 3.28)

project(TooDeeEditor LANGUAGES CXX)

message(STATUS "Setting up tests")

# Find Packages
find_package(GTest CONFIG REQUIRED)

# Test Targets
add_executable(
  hello_test
  hello_test.cpp
)
target_link_libraries(
  hello_test
  GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(hello_test)
