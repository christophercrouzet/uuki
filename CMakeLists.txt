cmake_minimum_required(VERSION 3.1)

project(
    Uuki
    VERSION 0.0.1
    LANGUAGES C
)

string(TOLOWER ${PROJECT_NAME} W_PROJECT)

# ------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# ------------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wpedantic -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual
        -Wconversion -Wfloat-equal -Wpointer-arith -Wshadow -Wstrict-overflow=5
        -Wswitch -Wswitch-default -Wundef -Wunreachable-code -Wwrite-strings
    )
endif()

# ------------------------------------------------------------------------------

set(W_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ------------------------------------------------------------------------------

set(W_LIBRARY_TARGETS)

macro(w_add_library target)
    set(W_ADD_LIBRARY_OPTIONS)
    set(W_ADD_LIBRARY_SINGLE_VALUE_ARGS)
    set(W_ADD_LIBRARY_MULTI_VALUE_ARGS FILES DEPENDS INCLUDES PROPERTIES)
    cmake_parse_arguments(
        W_ADD_LIBRARY
        "${W_ADD_LIBRARY_OPTIONS}"
        "${W_ADD_LIBRARY_SINGLE_VALUE_ARGS}"
        "${W_ADD_LIBRARY_MULTI_VALUE_ARGS}"
        ${ARGN}
    )

    add_library(
        ${target}
        ${W_ADD_LIBRARY_FILES}
    )
    set_target_properties(
        ${target}
        PROPERTIES
            LINKER_LANGUAGE C
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
            ARCHIVE_OUTPUT_DIRECTORY lib
            LIBRARY_OUTPUT_DIRECTORY lib
            ${W_ADD_LIBRARY_PROPERTIES}
    )
    target_include_directories(
        ${target}
        PUBLIC ${W_SOURCE_DIR}
        PRIVATE ${W_ADD_LIBRARY_INCLUDES}
    )
    target_link_libraries(
        ${target}
        PRIVATE ${W_ADD_LIBRARY_DEPENDS}
    )
    add_library(
        ${PROJECT_NAME}::${target}
        ALIAS ${target}
    )

    if (GNU OR Clang OR Intel)
        target_compile_options(
            ${target}
            PRIVATE -save-temps=obj
        )
    endif()

    if (UNIX)
        target_compile_definitions(
            ${target}
            PUBLIC
                _DEFAULT_SOURCE
                _POSIX_C_SOURCE=200112L
        )
        target_link_libraries(
            ${target}
            PRIVATE pthread
        )
    endif()

    list(APPEND W_LIBRARY_TARGETS ${target})
endmacro()

w_add_library(
    base
    FILES
        src/uuki/base/bit.c
        src/uuki/base/io.c
        src/uuki/base/logging.c
        src/uuki/base/math.c
        src/uuki/base/mem.c
        src/uuki/base/private/array.c
)

add_custom_target(
    libraries
    DEPENDS ${W_LIBRARY_TARGETS}
)

# ------------------------------------------------------------------------------

set(W_BINARY_TARGETS)

macro(w_add_binary target)
    set(W_ADD_BINARY_OPTIONS)
    set(W_ADD_BINARY_SINGLE_VALUE_ARGS)
    set(W_ADD_BINARY_MULTI_VALUE_ARGS FILES DEPENDS INCLUDES PROPERTIES)
    cmake_parse_arguments(
        W_ADD_BINARY
        "${W_ADD_BINARY_OPTIONS}"
        "${W_ADD_BINARY_SINGLE_VALUE_ARGS}"
        "${W_ADD_BINARY_MULTI_VALUE_ARGS}"
        ${ARGN}
    )

    add_executable(
        ${target}
        ${W_ADD_BINARY_FILES}
    )
    set_target_properties(
        ${target}
        PROPERTIES
            LINKER_LANGUAGE C
            RUNTIME_OUTPUT_DIRECTORY bin
            ${W_ADD_BINARY_PROPERTIES}
    )
    target_include_directories(
        ${target}
        PUBLIC ${W_SOURCE_DIR}
        PRIVATE ${W_ADD_BINARY_INCLUDES}
    )
    target_link_libraries(
        ${target}
        PRIVATE ${W_ADD_BINARY_DEPENDS}
    )
    add_executable(
        ${PROJECT_NAME}::${target}
        ALIAS ${target}
    )

    if (GNU OR Clang OR Intel)
        target_compile_options(
            ${target}
            PRIVATE -save-temps=obj
        )
    endif()

    list(APPEND W_BINARY_TARGETS ${target})
endmacro()

w_add_binary(
    uuki
    FILES
        src/uuki/cli/main.c
    DEPENDS
        base
)

add_custom_target(
    binaries
    DEPENDS ${W_BINARY_TARGETS}
)

# ------------------------------------------------------------------------------

option(REXO_BUILD_TESTS OFF)
add_subdirectory(deps/rexo)

set(W_TEST_TARGETS)

macro(w_add_test)
    set(W_ADD_TEST_OPTIONS)
    set(W_ADD_TEST_SINGLE_VALUE_ARGS NAME)
    set(W_ADD_TEST_MULTI_VALUE_ARGS FILES DEPENDS INCLUDES PROPERTIES)
    cmake_parse_arguments(
        W_ADD_TEST
        "${W_ADD_TEST_OPTIONS}"
        "${W_ADD_TEST_SINGLE_VALUE_ARGS}"
        "${W_ADD_TEST_MULTI_VALUE_ARGS}"
        ${ARGN}
    )

    add_executable(
        test-${W_ADD_TEST_NAME}
        ${W_ADD_TEST_FILES}
    )
    set_target_properties(
        test-${W_ADD_TEST_NAME}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
            OUTPUT_NAME ${W_ADD_TEST_NAME}
            ${W_ADD_TEST_PROPERTIES}
    )
    target_include_directories(
        test-${W_ADD_TEST_NAME}
        PRIVATE ${W_ADD_TEST_INCLUDES}
    )
    target_link_libraries(
        test-${W_ADD_TEST_NAME}
        PRIVATE
            rexo
            ${W_ADD_TEST_DEPENDS}
    )
    add_test(
        NAME ${W_ADD_TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test-${W_ADD_TEST_NAME}
    )

    if (GNU OR Clang OR Intel)
        target_compile_options(
            test-${W_ADD_TEST_NAME}
            PRIVATE -save-temps=obj
    )
    endif()

    list(APPEND W_TEST_TARGETS test-${W_ADD_TEST_NAME})
endmacro()

enable_testing()

w_add_test(
    NAME base-array
    FILES tests/base/array.c
    DEPENDS base
)

w_add_test(
    NAME base-io
    FILES tests/base/io.c
    DEPENDS base
)

w_add_test(
    NAME base-logging
    FILES tests/base/logging.c
    DEPENDS base
)

w_add_test(
    NAME base-macros
    FILES tests/base/macros.c
    DEPENDS base
)

w_add_test(
    NAME base-mem
    FILES tests/base/mem.c
    DEPENDS base
)

add_custom_target(tests DEPENDS ${W_TEST_TARGETS})
