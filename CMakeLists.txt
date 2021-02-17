cmake_minimum_required(VERSION 3.17)
project(c-demo C)

set(CMAKE_C_STANDARD 11)

add_subdirectory (src)

enable_testing ()
add_subdirectory (test)