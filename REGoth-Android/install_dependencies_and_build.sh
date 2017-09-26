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
mkdir -p android-sdk
unzip -q sdk-tools-linux-3859397.zip -d android-sdk
rm sdk-tools-linux-3859397.zip

# Setup environment
export ANDROID_HOME=`pwd`/android-sdk
export ANDROID_NDK_HOME=`pwd`/android-ndk-r14b-linux-x86_64
export LOCAL_ANDROID_NDK_HOME="$ANDROID_NDK_HOME"
export LOCAL_ANDROID_NDK_HOST_PLATFORM="linux-x86_64"
export PATH=$PATH:${ANDROID_HOME}
export PATH=$PATH:${ANDROID_HOME}/tools
export PATH=$PATH:${ANDROID_NDK_HOME}

# Setup Android SDK
echo "Updating SDK"
yes | ${ANDROID_HOME}/tools/bin/sdkmanager --update

echo "Downloading Android Build Tools"
yes | ${ANDROID_HOME}/tools/bin/sdkmanager "platforms;android-21" "build-tools;25.0.2" "extras;google;m2repository" "extras;android;m2repository"

echo "Andorid SDK Licenses..."
yes | ${ANDROID_HOME}/tools/bin/sdkmanager --licenses

popd # Leaving dependencies

./gradlew build

popd # Leaving script-folder
