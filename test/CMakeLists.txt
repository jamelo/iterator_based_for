include_directories(
    SYSTEM ${gtest_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

file(GLOB test_sources "*.cpp")

set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Weffc++ -pedantic -Wextra")

add_executable(iterator_based_for_test ${test_sources})
target_link_libraries(iterator_based_for_test gtest_main)
