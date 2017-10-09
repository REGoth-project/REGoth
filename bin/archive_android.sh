#!/bin/bash
set -e -x -o pipefail

rm -rf dist/
mkdir -p dist/
if [ -z "$PLATFORM" ]; then
    PLATFORM=$TRAVIS_OS_NAME
fi
FILENAME=REGoth-${TRAVIS_BRANCH}-${PLATFORM}-unsigned.apk
cp REGoth-Android/app/build/outputs/apk/app-arm-debug-unsigned.apk dist/${FILENAME}
