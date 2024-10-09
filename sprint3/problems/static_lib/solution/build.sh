#!/bin/bash

if [ "$1" == "clean" ]; then
  echo "clean build/"
  rm -rf ./build/*
fi

if ! [ -d "build" ]; then
  echo "mkdir -p build"
  mkdir -p build
fi

cd build

if ! [ -f "conanbuildinfo.cmake" ]; then
  echo "conan install .."
  conan install .. -s compiler.libcxx=libstdc++11 -s build_type=Debug --build=missing
fi

cmake ..
cmake --build .
