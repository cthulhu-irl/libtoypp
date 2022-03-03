cmake_minimum_required(VERSION 3.8)

project(libtoypp CXX)

set(CMAKE_CXX_STANDARD, 17)

include("cmake/setup_vcpkg.cmake")

option(LIBTOYPP_OPT_GENERATE_DOCS "Generate documentation"  FALSE)
option(LIBTOYPP_OPT_BUILD_TEST "Build and perform tests" TRUE)
option(LIBTOYPP_OPT_INSTALL "Generate and install libtoypp target" TRUE)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(LIBTOYPP_PUBLIC_HEADER_DIR "${PROJECT_SOURCE_DIR}/include/")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
  ${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(LIBTOYPP_OPT_GENERATE_DOCS)
#  add_subdirectory("docs")
endif()

if(LIBTOYPP_OPT_BUILD_TEST)
  enable_testing()
  add_subdirectory("tests")
endif()

if(LIBTOYPP_OPT_INSTALL)
endif()

#add_subdirectory("examples")
