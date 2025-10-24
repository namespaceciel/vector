cmake_minimum_required(VERSION 3.12)
project(ciel_vector LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose build type: Debug/Release/RelWithDebInfo/MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

option(CIEL_VECTOR_BUILD_TESTS      "Build unit tests" OFF)
option(CIEL_VECTOR_BUILD_BENCHMARKS "Build benchmarks" OFF)

add_library(ciel_vector INTERFACE)

target_include_directories(ciel_vector INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CIEL_VECTOR_BUILD_TESTS)
    add_subdirectory(test)
endif ()

if (CIEL_VECTOR_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif ()
