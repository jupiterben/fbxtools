#!/usr/bin/bash

mkdir build
cd build

VERBOSE=1
cmake .. "-DCMAKE_BUILD_TYPE=Debug"
cmake --build .
