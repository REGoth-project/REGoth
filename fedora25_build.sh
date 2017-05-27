
echo "Installing required components"

dnf install gcc gcc-c++ cmake cmake-data openal-soft-devel libxcb-devel xcb-util-devel xcb-util-image-devel libXcursor-devel

dnf install flac-devel libjpeg-turbo-devel freetype-devel glew-devel libXrandr-devel libsndfile-devel systemd-devel libXinerama-devel libvorbis-devel

echo "Remove previus build directory"
rm -rf build

echo "Building:"
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..

make -j4
