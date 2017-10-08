#!/bin/bash

# Stop on non-zero exit code
set -e
set -o pipefail

if [ "$PLATFORM" = "android" ]; then
    cd REGoth-Android/
    ./gradlew build
else
    if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi
    mkdir -p build;
    cd build;
    cmake ..;
    make -j4;
fi
