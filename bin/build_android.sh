#!/bin/bash
set -e -x -o pipefail

export TERM=dumb # Fixes gradle output looking bad

cd lib/libdmusic/utils/dls2sf/lib/libsndfile
./autogen.sh
./Scripts/android-configure.sh
make -j4
make install

cd ../../../../../../REGoth-Android/
# Release build should be signed, while debug signs automatically
./gradlew assembleDebug
