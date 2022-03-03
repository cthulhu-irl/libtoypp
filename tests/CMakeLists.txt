cmake_minimum_required(VERSION 3.8)

file(GLOB test_sources "*.cpp")

add_executable(tests ${test_sources})

target_compile_features(tests PRIVATE cxx_std_17)

target_compile_options(tests PRIVATE -g -O0 -Wall -Wextra -Wpedantic)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(tests PRIVATE
                      Catch2::Catch2
                      Catch2::Catch2WithMain
                      libtoypp::libtoypp)

include(CTest)
include(Catch)

catch_discover_tests(tests)
