cmake_minimum_required(VERSION 3.22.1)
project(Capstone)

# Language specs
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Linker & compiler flags
set(CAPNP_LIBS "-lcapnp -lcapnp-rpc -lkj -lkj-async")
set(CMAKE_REQUIRED_LIBRARIES "${CAPNP_LIBS} -lglog -pthread")

set(GEN_PROTO_FILES src/Server/generated/synchro.capnp.*)
add_custom_command(OUTPUT ${GEN_PROTO_FILES}
        COMMAND bash -c "./gen_proto"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/Server/"
        MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/src/Server/synchro.capnp
        VERBATIM)

# List source files and define target
file(GLOB_RECURSE SRCS_GAME   ${PROJECT_SOURCE_DIR}/src/Game/*)
file(GLOB_RECURSE SRCS_SERVER ${PROJECT_SOURCE_DIR}/src/Server/*)
file(GLOB_RECURSE SRCS ${SRCS_GAME} ${SRCS_SERVER})
file(GLOB_RECURSE LIBS ${PROJECT_SOURCE_DIR}/lib/*)
add_executable(Capstone ${PROJECT_SOURCE_DIR}/src/main.cpp ${GEN_PROTO_FILES} ${PROJECT_SOURCE_DIR}/config.h ${SRCS} ${LIBS})

# Add base directories for more intuitive includes
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/lib/)

# Propagate Debug mode by setting the `DEBUG` preprocessor macro
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(Capstone DEBUG)
endif ()

# Constants injected at compile time
add_compile_definitions(Capstone PROJECT_ROOT="${PROJECT_SOURCE_DIR}")

set(CMAKE_THREAD_LIBS_INIT "-pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_PTHREADS_INIT 1)
find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
target_link_libraries(Capstone Threads::Threads)

# Google Logging Library
target_link_libraries(Capstone glog)

find_package(CapnProto REQUIRED)
target_link_libraries(Capstone CapnProto::capnp)
target_link_libraries(Capstone CapnProto::capnp-rpc)
target_link_libraries(Capstone kj)
target_link_libraries(Capstone kj-async)

# Set OpenGL preference
set(OpenGL_GL_PREFERENCE GLVND)

# Oxygine Config
add_subdirectory(oxygine-framework)
add_definitions(${OXYGINE_DEFINITIONS})
include_directories(${OXYGINE_INCLUDE_DIRS})
link_directories(${OXYGINE_LIBRARY_DIRS})
target_link_libraries(Capstone ${OXYGINE_CORE_LIBS})

# Box2d
find_package(box2d REQUIRED)
include_directories(box2d/include/box2d)
target_link_libraries(Capstone box2d)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS} -fconcepts")

if (EMSCRIPTEN)
    SET(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
    em_link_pre_js(Game  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)

find_package(Catch2 3 CONFIG)
if (Catch2_FOUND)
    add_subdirectory(Catch2)
    file(GLOB_RECURSE TEST_SRCS ${PROJECT_SOURCE_DIR}/test/*.c*)
    add_executable(Test ${GEN_PROTO_FILES} ${TEST_SRCS} ${SRCS} ${LIBS})
    target_link_libraries(Test PRIVATE Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(Test)

    target_link_libraries(Test PRIVATE ${OXYGINE_CORE_LIBS})
    target_link_libraries(Test PRIVATE box2d)
    target_link_libraries(Test PRIVATE glog)

    target_link_libraries(Test PRIVATE Threads::Threads)

    target_link_libraries(Test PRIVATE CapnProto::capnp)
    target_link_libraries(Test PRIVATE CapnProto::capnp-rpc)
    target_link_libraries(Test PRIVATE kj)
    target_link_libraries(Test PRIVATE kj-async)

    add_compile_definitions(Test PRIVATE ERRORS_AS_WARNINGS)
endif()

# Simple Target compiling a client to test RPC server connections.
# TODO: remove
add_executable(Client client.cpp ${GEN_PROTO_FILES} ${SRCS_SERVER} ${LIBS})
target_link_libraries(Client PRIVATE glog)

target_link_libraries(Client PRIVATE CapnProto::capnp)
target_link_libraries(Client PRIVATE CapnProto::capnp-rpc)
target_link_libraries(Client PRIVATE kj)
target_link_libraries(Client PRIVATE kj-async)
