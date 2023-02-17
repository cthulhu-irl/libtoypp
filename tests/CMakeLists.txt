cmake_minimum_required(VERSION 3.8)

add_executable(tests)

target_sources(tests PRIVATE
    uniqueptr.cpp
    threaded_doublebuffer.cpp)

target_compile_features(tests PRIVATE cxx_std_17)

find_package(Catch2 CONFIG REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(tests PRIVATE
                      libtoypp::libtoypp
                      Threads::Threads
                      Catch2::Catch2
                      Catch2::Catch2WithMain)

if(LIBTOYPP_OPT_COVERAGE)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "code-coverage with non-Debug build is inaccurate.")
    endif()

    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(tests PRIVATE --coverage -O0 -g)
        target_link_libraries(tests PRIVATE --coverage)
    endif()

    message(STATUS "added compiler flags to generate coverage report.")
endif()

# to find and include(Catch)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(CTest)
include(Catch)

catch_discover_tests(tests)

