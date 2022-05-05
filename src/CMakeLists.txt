message(STATUS "enter src directory...")

file(GLOB_RECURSE skiplist_source ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
add_library(skiplist_shared SHARED ${skiplist_source})

######################################################################################################################
# THIRD-PARTY SOURCES
# murmur3
file(GLOB_RECURSE murmur3_sources
        ${PROJECT_SOURCE_DIR}/third_party/murmur3/*.cpp ${PROJECT_SOURCE_DIR}/third_party/murmur3/*.h)
add_library(thirdparty_murmur3 SHARED ${murmur3_sources})
target_link_libraries(skiplist_shared thirdparty_murmur3)