message(STATUS "enter test directory...")

file(GLOB SKIPLIST_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*test.cpp")
######################################################################################################################
# DEPENDENCIES

# valgrind
find_program(VALGRIND_BIN valgrind)
if ("${VALGRIND_BIN}" STREQUAL "VALGRIND_BIN-NOTFOUND")
    message(WARNING "skiplist/test couldn't find valgrind.")
else()
    message(STATUS "skiplist/test found valgrind at ${VALGRIND_BIN}")
endif()

set(VALGRIND_OPTIONS
        --error-exitcode=1                      # if leaks are detected, return nonzero value
        # --gen-suppressions=all  # uncomment for leak suppression syntax
        --leak-check=full                       # detailed leak information
        --soname-synonyms=somalloc=*jemalloc*   # also intercept jemalloc
        --trace-children=yes                    # trace child processes
        --track-origins=yes                     # track origin of uninitialized values
        )
set(VALGRIND_SUPPRESSIONS_FILE "${PROJECT_SOURCE_DIR}/build_support/valgrind.supp")

######################################################################################################################
# MAKE TARGETS
######################################################################################################################

##########################################
# "make check-tests"
##########################################
add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)
add_custom_target(check-tests COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

##########################################
# "make XYZ_test"
##########################################
foreach (skiplist_test_source ${SKIPLIST_TEST_SOURCES})
    # Create a human readable name.
    get_filename_component(skiplist_test_filename ${skiplist_test_source} NAME)
    string(REPLACE ".cpp" "" skiplist_test_name ${skiplist_test_filename})

    # Add the test target separately and as part of "make check-tests".
    add_executable(${skiplist_test_name} EXCLUDE_FROM_ALL ${skiplist_test_source})
    message(STATUS ${skiplist_test_name})
    add_dependencies(build-tests ${skiplist_test_name})
    add_dependencies(check-tests ${skiplist_test_name})

    target_link_libraries(${skiplist_test_name} skiplist_shared gtest gmock_main)

    # Set test target properties and dependencies.
    set_target_properties(${skiplist_test_name}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
        COMMAND ${skiplist_test_name}
    )

    # Add the test under CTest.
    add_test(${skiplist_test_name} ${VALGRIND_BIN} ${VALGRIND_OPTIONS}
    --suppressions=${VALGRIND_SUPPRESSIONS_FILE} ${CMAKE_BINARY_DIR}/test/${skiplist_test_name}
    --gtest_color=yes --gtest_output=xml:${CMAKE_BINARY_DIR}/test/unit_${test_name}.xml)
    add_test(${skiplist_test_name} ${CMAKE_BINARY_DIR}/test/${skiplist_test_name} --gtest_color=yes
            --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${skiplist_test_name}.xml)
endforeach(skiplist_test_source ${skiplist_TEST_SOURCES})
