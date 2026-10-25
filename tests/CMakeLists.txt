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

add_executable(
  project_loader_test
  project_loader_test.cpp
  ../apps/cli/src/Project.cpp
)
target_include_directories(project_loader_test PRIVATE ../apps/cli/include)
target_link_libraries(
  project_loader_test
  PRIVATE
  GTest::gtest_main
  unofficial::inih::inireader
  TooDeeEngine
)

include(GoogleTest)

gtest_discover_tests(hello_test)
gtest_discover_tests(project_loader_test)
