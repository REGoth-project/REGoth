#!/bin/bash
set -e -x -o pipefail

rm -rf dist/
mkdir -p dist/
if [ -z "$PLATFORM" ]; then
    PLATFORM=$TRAVIS_OS_NAME
fi
FILENAME=REGoth-${TRAVIS_BRANCH}-${PLATFORM}.tar.gz
tar -zcf dist/${FILENAME} -C build/bin REGoth shaders/
