#!/bin/bash

echo "Cleaning previous builds..."
rm -rf cmake-build-debug

echo "Creating build directory..."
mkdir cmake-build-debug
cd cmake-build-debug

echo "Running CMake..."
cmake -G "Unix Makefiles" ..

echo "Building the project..."
make
