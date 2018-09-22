#!/bin/bash
set -e -x -o pipefail

rm -rf dist/
mkdir -p dist/
if [ -z "$PLATFORM" ]; then
    PLATFORM=$TRAVIS_OS_NAME
fi
FILENAME=REGoth-${TRAVIS_BRANCH}-${PLATFORM}-debug.apk
cp REGoth-Android/app/build/outputs/apk/arm/debug/app-arm-debug.apk dist/${FILENAME}
