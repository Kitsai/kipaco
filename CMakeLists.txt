cmake_minimum_required(VERSION 3.20...3.27.0)
project(cpp-parser-combinator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "" FORCE)
endif()

if(WIN32)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

add_subdirectory(kipaco)
add_subdirectory(tests)
