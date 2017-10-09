#!/bin/bash
set -e -x -o pipefail

export TERM=dumb # Fixes gradle output looking bad

cd REGoth-Android/
./gradlew task
./gradlew assembleDebug
