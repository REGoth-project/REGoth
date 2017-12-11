#!/bin/bash
set -e -x -o pipefail

if [ "$CXX" = "g++" ]; then export CXX="g++-6" CC="gcc-6"; fi

mkdir -p build-libsndfile
mkdir -p $TRAVIS_BUILD_DIR/sndfile_install
cd build-libsndfile
cmake -DCMAKE_INSTALL_PREFIX=$TRAVIS_BUILD_DIR/sndfile_install ../lib/libdmusic/utils/dls2sf/lib/libsndfile
make -j4
cd ../

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$TRAVIS_BUILD_DIR/sndfile_install ..
make -j4
