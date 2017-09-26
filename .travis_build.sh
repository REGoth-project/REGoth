#!/bin/bash

# Stop on non-zero exit code
set -e
set -o pipefail

if [ "$PLATFORM" = "android" ]; then
    REGoth-Android/install_dependencies_and_build.sh;
    [[ $? -ne 0 ]] && exit 1; # Exit if non-zero exit code
else
    if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi
    git submodule update --init --recursive;
    mkdir -p build;
    cd build;
    cmake ..;
    make -j2;
fi
