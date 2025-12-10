# Copyright (c) 2025 Hüseyin Karakaya
# This file is a part of the Mizan project
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.15)

project(Mizan
  VERSION 0.1.0
  DESCRIPTION "B-Rep Geometric Modeling Kernel"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Use: cmake -B build")
endif()

option(MIZAN_BUILD_TESTS "Build unit tests" ON)
option(MIZAN_BUILD_EXAMPLES "Build examples" OFF)

add_subdirectory(src)

if(MIZAN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(MIZAN_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

message(STATUS "")
message(STATUS "Mizan v${PROJECT_VERSION}")
message(STATUS "  Compiler:   ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build:      ${CMAKE_BUILD_TYPE}")
message(STATUS "  Tests:      ${MIZAN_BUILD_TESTS}")
message(STATUS "  Examples:   ${MIZAN_BUILD_EXAMPLES}")
message(STATUS "")