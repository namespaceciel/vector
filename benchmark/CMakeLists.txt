cmake_minimum_required(VERSION 3.12)
project(ciel_vector_benchmark LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(benchmark)

add_executable(ciel_vector_benchmark
    vector.cpp
)

target_link_libraries(ciel_vector_benchmark PRIVATE
    ciel_vector
    benchmark::benchmark_main
)

target_compile_options(ciel_vector_benchmark PRIVATE -O3 -Wall -Wextra)
