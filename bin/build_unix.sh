#!/bin/bash
set -e -x -o pipefail

if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
