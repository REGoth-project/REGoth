pushd "$(dirname "$0")"

mkdir -p android-dependencies

pushd android-dependencies

# Download Android SDK and NDK
curl -L https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip -O
curl -L https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip -O


# Install NDK
unzip -q android-ndk-r14b-linux-x86_64.zip
rm android-ndk-r14b-linux-x86_64.zip

# Install SDK
unzip -q sdk-tools-linux-3859397.zip
rm sdk-tools-linux-3859397.zip

# Setup environment
export ANDROID_HOME=`pwd`/sdk-tools-linux-3859397
export ANDROID_NDK_HOME=`pwd`/android-ndk-r14b-linux-x86_64
export LOCAL_ANDROID_NDK_HOME="$ANDROID_NDK_HOME"
export LOCAL_ANDROID_NDK_HOST_PLATFORM="linux-x86_64"
export PATH=$PATH:${ANDROID_SDK_HOME}
export PATH=$PATH:${ANDROID_NDK_HOME}

popd # Leaving dependencies
popd # Leaving script-folder
