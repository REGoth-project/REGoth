#!/bin/bash

# Stop on non-zero exit code
set -e
set -o pipefail

if [ "$PLATFORM" = "android" ]; then
    export TERM=dumb # Fixes gradle output looking bad
    cd REGoth-Android/
    ./gradlew task
    ./gradlew assembleRelease
else
    if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi
    mkdir -p build;
    cd build;
    cmake ..;
    make -j4;
fi
