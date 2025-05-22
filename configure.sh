# #/bin/bash/
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

if [ "$#" -ne 1 ]; then
	BUILD_DIR="./build"
else
	BUILD_DIR="$1"
fi

# Check if the script is already in the build directory
if [ "$(basename "$(pwd)")" != "build" ]; then
  mkdir -p "$BUILD_DIR"
fi

# Check if the user provided the "mingw" argument
if [ "$1" == "mingw" ]; then
  # Use MinGW compiler
  COMPILER="$MINGW_PATH/bin/g++"
  # Run CMake with the MinGW toolchain file
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
				-DCMAKE_BUILD_TYPE="$TYPE" \
				-DCMAKE_CXX_COMPILER="$COMPILER" \
				-DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
        -S . \
        -B "$BUILD_DIR"
        # -DCMAKE_TOOLCHAIN_FILE="$MINGW_PATH/mingw$ARCH.cmake" \
else
  # Use default compiler
  COMPILER="$DEFAULT_COMPILER"
  # Run CMake with default compiler
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
				-DCMAKE_BUILD_TYPE="$TYPE" \
				-DCMAKE_CXX_COMPILER="$DEFAULT_COMPILER" \
				-DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
				-DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=mold" \
				-G "$GENERATOR" \
				-S . \
				-B "$BUILD_DIR"
fi

echo "Using compiler: $COMPILER"
