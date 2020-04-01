cmake_minimum_required(VERSION 3.5)

project(scrabble CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_library(cxx_project_options INTERFACE)
target_compile_features(cxx_project_options INTERFACE cxx_std_17)
include(cmake/Sanitizers.cmake)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- enabling sanitizers")
    enable_sanitizers(cxx_project_options)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_subdirectory(src)
