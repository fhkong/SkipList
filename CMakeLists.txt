cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

set(BUILD_SHARED_LIBS ON)
project(
  SkipList 
  VERSION 2022.5
  DESCRIPTION "The SkipList Project"
  LANGUAGES C CXX
  )

# enable c++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# guarantee make the project in independent mode
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
  message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build && cd build; cmake ..\" \
    Some junk files were created in this folder(CMakeCache.txt, CMakeFiles); you should delete those.")  
endif()


# enable CTest
enable_testing()

#########################################################################
# BUILD GTEST
# gmock gtest https://crascit.com/2015/07/25/cmake-gtest/
configure_file("${PROJECT_SOURCE_DIR}/build_support/gtest_CMakeLists.txt.in" googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # don't override our compiler/linker options when building gtest
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src" "${CMAKE_BINARY_DIR}/googletest-build")

###########################################################
# COMPLIER SETUP 
# Complier flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -Werror -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-attributes") #TODO: remove
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

set(GCC_COVERAGE_LINK_FLAGS    "-fPIC")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
# Output directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Includes. 
set(SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
set(TEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/test/include)
set(THIRD_PARTY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party)
include_directories(${THIRD_PARTY_INCLUDE_DIR} ${SRC_INCLUDE_DIR} ${TEST_INCLUDE_DIR})
include_directories(BEFORE src)
include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)
# Build subdirectories
add_subdirectory(src)
add_subdirectory(test)


##########################################################################
## build `make cpplint` command to check the source code depend Google C/C++ style
set(PROJECT_CPPLINT_SEARCH_DIR ${PROJECT_SOURCE_DIR}/build_support)
find_program(CPPLINT_BIN
  NAMES cpplint cpplint.py
  HINTS ${PROJECT_CPPLINT_SEARCH_DIR})
if ("${CPPLINT_BIN}" STREQUAL "CPPLINT_BIN-NOTFOUND")
  message(WARNING "The project couldn't found cpplint.py")
else() 
  message(STATUS "The project found cpplint at ${CPPLINT_BIN}")
endif()

# Run `make cpplint` to check source code
file(GLOB_RECURSE LINT_FILES
  ${CMAKE_SOURCE_DIR}/src/*.h
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/test/*.h
  ${CMAKE_SOURCE_DIR}/test/*.cpp)
add_custom_target(
  cpplint echo '${LINT_FILES}' | xargs -n12 -P8
  ${CPPLINT_BIN}
  --verbose=2 --quiet
  --linelength=120
  --filter=-legal/copyright
)


set(BUILD_SUPPORT_DIR ${CMAKE_SOURCE_DIR}/build_support)
##############################################################################
## build `make format` command to format source code depend Google C/C++ style
string(CONCAT FORMAT_DIRS
  "${CMAKE_SOURCE_DIR}/src,"  
  "${CMAKE_SOURCE_DIR}/test,"  
)
set(PROJECT_CLANG_SEARCH_DIR "/usr/bin" "/usr/local/bin" "/usr/local/llvm-8/bin")
find_program( CLANG_FORMAT_BIN
  NAMES clang-format clang-format-8
  HINTS ${PROJECT_CLANG_SEARCH_DIR}
)
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
  message(WARNING "The project couldn't found clang-format")
else() 
  message(STATUS "The project found clang-format at ${CLANG_FORMAT_BIN}")
endif()
# Run `make format` to format source code and test code
add_custom_target(
  format
  ${CMAKE_SOURCE_DIR}/build_support/run_clang_format.py
  ${CLANG_FORMAT_BIN}
  ${CMAKE_SOURCE_DIR}/build_support/clang_format_exclusions.txt
  --source_dirs
  ${FORMAT_DIRS}
  --fix
  # --quiet
)
# Run `make check-format` to check format not updates files in place.
add_custom_target(
  check-format
  ${CMAKE_SOURCE_DIR}/build_support/run_clang_format.py
  ${CLANG_FORMAT_BIN}
  ${CMAKE_SOURCE_DIR}/build_support/clang_format_exclusions.txt
  --source_dirs
  ${FORMAT_DIRS}
  #--quiet
)

# ################################################################################
# THIS PART NEEDS PROJECT TO BE DONE!!! BECAUSE IT'S NEED SHARED LIBRARY
# ## build `make clang-tidy` to check memory leak problem
# find_program(
#   CLANG_TIDY_BIN
#   NAMES clang-tidy clang-tidy-8
#   HINTS ${PROJECT_CLANG_SEARCH_DIR}
# )
# if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
#     message(WARNING "The project couldn't find clang-tidy.")
# else()
#     # Output compile_commands.json
#     set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
#     message(STATUS "The project found clang-tidy at ${CLANG_TIDY_BIN}")
# endif()
# # Run `make check-clang-tidy` looks for checking memory leak.
# # note that clang-tidy automatically looks for a .clang-tidy file in parent directories
# add_custom_target(
#   check-clang-tidy
#   ${CMAKE_SOURCE_DIR}/build_support/run_clang_tidy.py
#   -clang-tidy-binary ${CLANG_TIDY_BIN}     # using our clang-tidy binary
#   -p ${CMAKE_BINARY_DIR}      # using cmake's generated compile commands
# )
# add_dependencies(check-clang-tidy gtest skiplist_shared) #needs gtest headers and skiplist shared library 