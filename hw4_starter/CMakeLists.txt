cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(hw4 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_library(tests_main tests_main.cpp)

add_executable(to_fix to_fix.cpp)
target_link_libraries(to_fix tests_main)

add_executable(to_complete to_complete.cpp)
target_link_libraries(to_complete tests_main)

enable_testing()

add_test(to_fix to_fix)
add_test(to_complete to_complete)
