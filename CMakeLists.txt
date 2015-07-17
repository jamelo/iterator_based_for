cmake_minimum_required(VERSION 2.6)
project(iterator_based_for)

add_subdirectory(gtest-1.7.0)
add_subdirectory(test)

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Weffc++ -pedantic -Wextra")

include_directories(include)

add_executable(iterator_based_for main.cpp)

install(TARGETS iterator_based_for RUNTIME DESTINATION bin)
