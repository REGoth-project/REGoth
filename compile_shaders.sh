#!/usr/bin/env bash
set -e

# usage: ./compile_fshader.sh <shader files>

# the following variables must be set:
# - SHADERC: location of shaderc (shader compiler)
# - BUILD_DIR: directory where the binary shaders shall be created

# location of script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SHADER_BACKEND=glsl
BGFX_DIR=$DIR/lib/bgfx-cmake/bgfx/

BUILD_DIR=$BUILD_DIR/shaders/$SHADER_BACKEND

if [ -z ${SHADERC+x} ]; then
    (>&2 echo "error: variable SHADERC must be set and point to the shaderc compiler location")
    exit 1
elif [ ! -f "$SHADERC" ]; then
    (>&2 echo "error: file $SHADERC not found")
    exit 1
fi

for fullpath in "$@"
do
    filename=$(basename "$fullpath")
    if [[ $filename == fs_*.sc ]]; then
        type=fragment
    elif [[ $filename == vs_*.sc ]]; then
        type=vertex
    else
        (>&2 echo "error: unknown shader file prefix for file: $fullpath")
        exit 1
    fi
    mkdir -p "$BUILD_DIR"
    SHADER_BIN_NAME="${filename%.sc}.bin"
    echo -n "Building shader $SHADER_BIN_NAME ..."

    "$SHADERC" --platform linux -p 120 --type $type --depends -i "$BGFX_DIR/src/" -o "$BUILD_DIR/$SHADER_BIN_NAME" -f "$fullpath" --disasm -i "$BGFX_DIR/examples/common/"
    echo "successful!"
done

