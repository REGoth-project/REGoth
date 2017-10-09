#!/bin/bash

set -e
set -o pipefail

export TERM=dumb # Fixes gradle output looking bad
cd REGoth-Android/
./gradlew task
./gradlew assembleRelease
