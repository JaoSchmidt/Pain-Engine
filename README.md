# Pain-Engine

Pain-Engine is a C++ game engine focused on fast iteration, ECS-driven architecture, and batched 2D/3D rendering.
It provides built-in systems for rendering, scripting (Lua + native), physics, asset management, and tooling.

This repository contains the engine and example game entry points.

### ✨ Features

- ImGui **Editor** and **Launcher**
- Event **Bus/Obsersvable**
- **Profiling** tool, which can be used inside chrome://tracing
- **Cross-platform** build (Linux / Windows)
- Doxygen **Documentation**
- **Texture**, **font**, and other asset managers, including:
- - **ECS-based** architecture with **opt-in** systems
- - **Batched 2D** renderer _system_. 3D renderer comming soon
- - **Native** and **Lua** scripting _systems_
- - **Collision**, **kinematics** _systems_
- - **Schedule** _system_

### ⚙️ Development:

#### 📦 Requirements:

- Git
- CMake (≥ 3.31)
- C++ compiler with C++20 support
- Vcpkg
- ninja or make

#### Linux additional requirements to build painlessly:

You can assume most of them will already be on to your system if you are using a desktop. But this might be necessary to compile on servers

```
sudo apt-get install -y \
  rsync \
  ccache \
  git \
  cmake \
  make \
  pkg-config \
  zlib1g \
  bzip2 \
  gzip \
  libtool \
  autoconf \
  unzip \
  wget \
  libpng-dev \
  libbz2-dev \
  libharfbuzz-dev \
  libbrotli-dev \
  mesa-utils \
  libglu1-mesa-dev \
  mesa-common-dev \
  libasound2-dev \
  libxext-dev
```

### 📥 Cloning

It is recommended to clone with submodules as it will decrease the painfull build time later:

```bash
git clone --recursive https://github.com/JaoSchmidt/Pain-Engine.git
cd Pain-Engine
```

If you already cloned without submodules:

```
git submodule update --init --recursive
```

### 🛠️ Building

CMake presets offer a painless, automatic way to download external libraries.

Linux:

```bash
cmake --preset default
cd build
make
```

Windows:

```bash
cmake --preset ci-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build-ci-windows --config Debug
```

### 🎮 Selecting / Creating a Game

Pain-Engine automatically builds a game entry when it's located inside `Example/<GAME FOLDER>`.  For more info, check the section "Game Compilation" inside `CMakeLists.txt`.

### ▶️ Example usage

It is strongly recommended that you copy the creatively named `GameTemplate` since it already has a `CMakeLists.txt` file, which is used to instruct the compilation script.

```
cp -r Example/GameTemplate Example/MyGameFolder
cd Example/MyGameFolder
```
From there I also recommend changing the variable "PROJECT_NAME" to name it as you wish

### 🧪 Continuous Integration

This project is continuously built on:

- Ubuntu 24.04
- Windows (MSVC + vcpkg)

The CI workflows ensure the engine builds cleanly on both platforms using the same CMake presets. If there is any difficulty, you can look inside `./.github/workflows`, or do a `git checkout` on a previous working commit 👍.

### 📜 License

This project is licensed under the Mozilla Public License 2.0 (MPL-2.0).
See the LICENSE file for details.
