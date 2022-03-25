cmake_minimum_required(VERSION 3.22.1)
project(Capstone)

# Language specs
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)

# Linker & compiler flags
set(CMAKE_REQUIRED_LIBRARIES "-lglog")
set(CMAKE_CXX_FLAGS "-fconcepts")

# List source files and define target
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*)
list(REMOVE_ITEM SRCS ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(Capstone ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/config.h ${SRCS})

include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/test/)
include_directories(${PROJECT_SOURCE_DIR}/)

# Define dependencies
## Google Logging Library
target_link_libraries(Capstone glog)

# If Catch2 is installed prepare the `Test` target
find_package(Catch2 CONFIG)
IF (Catch2_FOUND)
    file(GLOB_RECURSE TEST_SRCS ${PROJECT_SOURCE_DIR}/test/*.c*)
    add_executable(Test ${GEN_PROTO_FILES} ${TEST_SRCS} ${SRCS})

    add_compile_definitions(Test PRIVATE ERRORS_AS_WARNINGS)

    target_link_libraries(Test PRIVATE Catch2::Catch2)
    include(CTest)
    include(Catch)
    catch_discover_tests(Test)

    target_link_libraries(Test PRIVATE glog)
ENDIF()
