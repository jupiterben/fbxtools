#!/usr/bin/bash

mkdir build
cd build

VERBOSE=1
cmake .. "-DCMAKE_BUILD_TYPE=Debug"
cmake --build .
cp src/fbx2json/Release/fbx2json.exe fbx2json.exe