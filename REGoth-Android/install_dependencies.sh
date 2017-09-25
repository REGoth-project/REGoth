pushd "$(dirname "$0")"

mkdir -p android-dependencies

pushd android-dependencies

# Download Android NDK
curl -L https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip -O

# Install NDK
unzip -q android-ndk-r14b-linux-x86_64.zip
rm android-ndk-r14b-linux-x86_64.zip

# Setup environment
export ANDROID_NDK_HOME=`pwd`/android-ndk-r14b-linux-x86_64
export LOCAL_ANDROID_NDK_HOME="$ANDROID_NDK_HOME"
export LOCAL_ANDROID_NDK_HOST_PLATFORM="linux-x86_64"
export PATH=$PATH:${ANDROID_NDK_HOME}

popd # Leaving dependencies
popd # Leaving script-folder
