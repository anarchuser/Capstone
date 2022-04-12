cmake_minimum_required(VERSION 3.22.1)
project(Capstone)

# Language specs
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

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

# Google Logging Library
target_link_libraries(Capstone glog)

# Set OpenGL preference
set(OpenGL_GL_PREFERENCE GLVND)

# Oxygine Config
add_subdirectory(oxygine-framework oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})
target_link_libraries(Capstone ${OXYGINE_CORE_LIBS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS}")

if (EMSCRIPTEN)
    SET(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
    em_link_pre_js(Game  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)

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
