cmake_minimum_required(VERSION 3.12)
project(ciel_vector_test LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")

enable_testing()

file(GLOB_RECURSE SOURCES "src/*.cpp")
set(ALL_TESTS "")

foreach(SOURCE ${SOURCES})
    get_filename_component(TEST_NAME ${SOURCE} NAME_WE)
    list(APPEND ALL_TESTS ${TEST_NAME})

    add_executable(${TEST_NAME} ${SOURCE})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(${TEST_NAME} PRIVATE ciel_vector)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)

    message(STATUS "Add test: ${TEST_NAME} <- ${SOURCE}")
endforeach()

add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${ALL_TESTS}
    COMMENT "Run all tests..."
)
