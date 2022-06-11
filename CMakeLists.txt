cmake_minimum_required(VERSION 3.23.1)
project(Capstone)

# Language specs
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Linker & compiler flags
set(CAPNP_LIBS "-lcapnp -lcapnp-rpc -lkj -lkj-async")
set(CMAKE_CXX_FLAGS "${CAPNP_LIBS} -pthread --std=c++20")

set(CAPNP_DIR "${PROJECT_SOURCE_DIR}/lib/Network")
set(CAPNP_GEN "${CAPNP_DIR}/generated/synchro.capnp.*")
add_custom_command(OUTPUT ${CAPNP_GEN}
        COMMAND bash -c "./gen_proto"
        WORKING_DIRECTORY "${CAPNP_DIR}"
        MAIN_DEPENDENCY "${CAPNP_DIR}/synchro.capnp"
        VERBATIM)

# List source files and define target
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE LIBS ${PROJECT_SOURCE_DIR}/lib/*)
file(GLOB_RECURSE TESTS ${PROJECT_SOURCE_DIR}/test/*.c*)
set(LIBS ${LIBS} ${CAPNP_GEN})
add_executable(Capstone ${PROJECT_SOURCE_DIR}/main.cpp ${PROJECT_SOURCE_DIR}/config.h ${SRCS} ${LIBS})

# Add base directories for more intuitive includes
target_include_directories(Capstone PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(Capstone PUBLIC ${PROJECT_SOURCE_DIR}/src/)
target_include_directories(Capstone PUBLIC ${PROJECT_SOURCE_DIR}/lib/)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OXYGINE_CXX_FLAGS} -fconcepts -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

if (EMSCRIPTEN)
    SET(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1 -s WASM=0 -s WARN_ON_UNDEFINED_SYMBOLS=1 --memory-init-file 0 -s TOTAL_MEMORY=50331648 -s FORCE_FILESYSTEM=1")
    em_link_pre_js(Game  ${OXYGINE_JS_LIBRARIES}  ${CMAKE_CURRENT_SOURCE_DIR}/data.js)
endif(EMSCRIPTEN)

find_package(Catch2 3 CONFIG)
if (Catch2_FOUND)
    add_subdirectory(Catch2)
    add_executable(Test ${TESTS} ${SRCS} ${LIBS} ${PROJECT_SOURCE_DIR}/config.h)

    target_include_directories(Test PRIVATE ${PROJECT_SOURCE_DIR}/lib/)
    target_include_directories(Test PRIVATE ${PROJECT_SOURCE_DIR}/src/)
    target_include_directories(Test PRIVATE ${PROJECT_SOURCE_DIR}/)

    target_link_libraries(Test PRIVATE Catch2::Catch2WithMain)
    include(Catch)
    include(CTest)
    catch_discover_tests(Test)

    target_link_libraries(Test PRIVATE ${OXYGINE_CORE_LIBS})
    target_link_libraries(Test PRIVATE box2d)

    target_link_libraries(Test PRIVATE Threads::Threads)

    target_link_libraries(Test PRIVATE CapnProto::capnp)
    target_link_libraries(Test PRIVATE CapnProto::capnp-rpc)
    target_link_libraries(Test PRIVATE kj)
    target_link_libraries(Test PRIVATE kj-async)
endif()
