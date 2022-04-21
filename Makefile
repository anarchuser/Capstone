# Simple Makefile providing an interface for cmake. Oh, sweet irony

BUILD_DIR  ?= build
BUILD_TYPE ?= Release

.PHONY = all build clean debug default run setup test

default:	run

# Clean, build, test, and run
all:	clean	test	run

# Initialise cmake build files at BUILD_DIR
setup:	CMakeLists.txt
	cmake -H. -B$(BUILD_DIR) -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" > /dev/zero

# Build the project (lib and src)
build:	setup
	cmake --build $(BUILD_DIR) --target Capstone

# Build the benchmarking tool
run:	build
	./$(BUILD_DIR)/Capstone

# Build and run the tests
test:	build
	cmake --build $(BUILD_DIR) --target Test
	./$(BUILD_DIR)/Test

# Clean all build files
clean:
	rm -rf $(BUILD_DIR)
