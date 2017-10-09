#!/bin/bash

set -e
set -o pipefail

if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi
mkdir -p build;
cd build;
cmake ..;
make -j4;
