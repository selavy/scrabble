cmake_minimum_required(VERSION 3.5)

project(scrabble C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

include(cmake/CompilerWarnings.cmake)
add_library(cxx_project_warnings INTERFACE)
add_library(c_project_warnings   INTERFACE)
set_cxx_project_warnings(cxx_project_warnings)
set_c_project_warnings(c_project_warnings)

add_library(cxx_project_options INTERFACE)
add_library(c_project_options   INTERFACE)
target_compile_features(cxx_project_options INTERFACE cxx_std_17)
target_compile_features(c_project_options   INTERFACE c_std_99)
include(cmake/Sanitizers.cmake)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("-- enabling sanitizers")
    enable_sanitizers(cxx_project_options)
    enable_sanitizers(c_project_options)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_package(ZLIB REQUIRED)

find_package(Libedit REQUIRED)
add_library(LibEdit INTERFACE)
target_link_libraries(LibEdit INTERFACE ${LIBEDIT_LIBRARIES})
target_include_directories(LibEdit INTERFACE ${LIBEDIT_INCLUDE_DIR})

find_package(Threads REQUIRED)

add_subdirectory(third_party)
add_subdirectory(mafsa)
add_subdirectory(cicero)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(ui)
