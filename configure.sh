#!/bin/bash

# Set the default compiler
DEFAULT_COMPILER="clang++"

# Set the path to your MinGW installation
MINGW_PATH="/usr/x86_64-w64-mingw32"

# Set the architecture (either "x86_64" or "i686")
ARCH="x86_64"

# Set the CMake generator for MinGW
# GENERATOR='Unix Makefiles'
GENERATOR="Ninja"
TYPE="Debug"
COMPILER_LAUNCHER="ccache"
# COMPILER_LAUNCHER="distcc"

# Default build directory
BUILD_DIR="./build"

# Parse argument (default: none)
ARG="${1:-}"

# CICD mode is enabled only if explicitly passed
CICD=false
if [ "$ARG" == "CICD" ]; then
  CICD=true
fi

# Ensure build directory exists
if [ "$(basename "$(pwd)")" != "build" ]; then
  mkdir -p "$BUILD_DIR"
fi

if [ "$CICD" == true ]; then
  mkdir -p "dkr_build"
  echo "🚀 Running in CICD (GitHub Actions) mode..."
  COMPILER="$DEFAULT_COMPILER"
  cmake -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER="g++" \
        -G "Unix Makefiles" \
        -S . \
        -B "dkr_build"
elif [ "$ARG" == "mingw" ]; then
  echo "🔧 Running with MinGW toolchain..."
  COMPILER="$MINGW_PATH/bin/g++"
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_BUILD_TYPE="$TYPE" \
        -DCMAKE_CXX_COMPILER="$COMPILER" \
        -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
        -S . \
        -B "$BUILD_DIR"
else
  echo "💻 Running with default compiler..."
  COMPILER="$DEFAULT_COMPILER"
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_BUILD_TYPE="$TYPE" \
        -DCMAKE_CXX_COMPILER="$DEFAULT_COMPILER" \
        -DCMAKE_CXX_COMPILER_LAUNCHER=$COMPILER_LAUNCHER \
        -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
        -G "$GENERATOR" \
        -S . \
        -B "$BUILD_DIR"
fi

echo "Using compiler: $COMPILER"

