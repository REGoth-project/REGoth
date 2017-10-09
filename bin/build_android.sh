#!/bin/bash
set -e -x -o pipefail

export TERM=dumb # Fixes gradle output looking bad

cd REGoth-Android/
# Release build should be signed, while debug signs automatically
./gradlew assembleDebug
