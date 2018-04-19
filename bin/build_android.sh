#!/bin/bash
set -e -x -o pipefail

export TERM=dumb # Fixes gradle output looking bad

mkdir build-libsndfile
cd build-libsndfile

cmake -DCMAKE_SYSTEM_NAME=Android -DBUILD_PROGRAMS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF \
        -DBUILD_REGTEST=OFF -DANDROID_NDK=$ANDROID_NDK_HOME -DDISABLE_EXTERNAL_LIBS=On \
        -DCMAKE_INSTALL_PREFIX=$ANDROID_NDK_HOME/sysroot ../lib/libdmusic/utils/dls2sf/lib/libsndfile
make -j4
make install

cd ../REGoth-Android/
# Release build should be signed, while debug signs automatically
./gradlew assembleDebug
