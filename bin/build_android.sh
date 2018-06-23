#!/bin/bash
set -e -x -o pipefail

export TERM=dumb # Fixes gradle output looking bad

mkdir build-libsndfile
cd build-libsndfile

cmake -DCMAKE_SYSTEM_NAME=Android -DBUILD_PROGRAMS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF \
        -DBUILD_REGTEST=OFF -DANDROID_NDK=$ANDROID_NDK_HOME -DDISABLE_EXTERNAL_LIBS=On \
        -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a \
        -DCMAKE_INSTALL_PREFIX=$ANDROID_NDK_HOME/sysroot ../lib/libdmusic/utils/dls2sf/lib/libsndfile
make -j4
make install

mkdir -p ../REGoth-Android/app/src/main/jniLibs/armeabi-v7a
cp $ANDROID_NDK_HOME/sysroot/lib/libsndfile.so ../REGoth-Android/app/src/main/jniLibs/armeabi-v7a/libsndfile.so

cd ../REGoth-Android/
# Release build should be signed, while debug signs automatically
./gradlew assembleDebug
