# Pain-Engine

Pain-Engine is a C++ game engine focused on fast iteration, ECS-driven architecture, and batched 2D/3D rendering.
It provides built-in systems for rendering, scripting (Lua + native), physics, asset management, and tooling.

This repository contains the engine and example game entry points.

### ✨ Features

- ImGui **Editor** and **Launcher**
- Event **Bus/Obsersvable**
- **Profiling** tool
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
cmake --preset ci
cd build-ci
make
```

Windows:

```bash
cmake --preset ci-windows -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build-ci-windows --config Debug
```

### 🎮 Selecting / Creating a Game

Pain-Engine builds a game entry point defined in the top-level `CMakeLists.txt`.

To change that open `CMakeLists.txt` and search for `$GAME_FOLDER` variable:

```
set(GAME_FOLDER "your/game/folder/here")
```

Change this path to point to your game entry folder.

### ▶️ Example usage

The most recent game is the creatively named "scriptGame" where `$GAME_FOLDER` should be pointing at the moment. You can use it as a template to create your own game.

If not, here is a recommended initial setup:

```cpp
pain::Application *pain::createApplication()
{
  // Retrieve the context the player will alter when using the launcher
  IniConfig ini;
  ini.readAndUpdate();

  // Retrieve the app context defined inside "resources/InternalConfig.ini"
  InternalConfig internalIni;
  internalIni.readAndUpdate(ini.assetsPath.value.c_str());

  // Create the application + OpenGL + Event contexts
  Application *app = Application::createApplication(         //
      {.title = internalIni.title.get().c_str(),             //
       .defaultWidth = ini.defaultWidth.get(),               //
       .defaultHeight = ini.defaultHeight.get()},            //
      {.swapChainTarget = internalIni.swapChainTarget.get()} //
  );

  // Create the ECS World Scene
  pain::Scene &scene = app->createWorldSceneComponents(
      internalIni.gridSize.get(), pain::NativeScriptComponent{});

  // Individually add each system
  scene.addSystem<Systems::SweepAndPruneSys>();
  scene.addSystem<Systems::Render>();
  scene.addSystem<Systems::NativeScript>();
  scene.addSystem<Systems::LuaScript>();
  scene.addSystem<Systems::Kinematics>();
  scene.addSystem<Systems::LuaSchedulerSys>();

  // (Optional) Defining a small native script (MainScript) for the world scene that will be executed on. Must have added System::NativeScript
  MainScript::createScriptScene(     //
      scene, ini.defaultWidth.get(), //
      ini.defaultHeight.get(),       //
      internalIni.zoomLevel.get(),   //
      app                            //
  );

  // (Optional) Creating the ECS UI scene
  UIScene &uiScene = app->createUIScene(pain::ImGuiComponent{});
  // (Optional) A small native script that works as our game engine editor
  UIScene::emplaceImGuiScript<PainlessEditor>(uiScene.getEntity(), uiScene,
                                              *app);

  return app;
}

#ifdef PLATFORM_IS_LINUX
int main(int argc, char *argv[])
#elif defined PLATFORM_IS_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow)
#endif
{
  bool isSettingsGuiNeeded = pain::Pain::initiateIni();
  EndGameFlags flags;
  flags.restartGame = !isSettingsGuiNeeded;
  if (isSettingsGuiNeeded) {
    pain::Application *app = pain::createLauncher();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  while (flags.restartGame) {
    pain::Application *app = pain::createApplication();
    flags = pain::Pain::runAndDeleteApplication(app);
  }
  return 0;
}
```

### 🧪 Continuous Integration

This project is continuously built on:

- Ubuntu 24.04
- Windows (MSVC + vcpkg)

The CI workflows ensure the engine builds cleanly on both platforms using the same CMake presets. If there is any difficulty, you can look inside `./.github/workflows`, or do a `git checkout` on a previous working commit 👍.

### 📜 License

This project is licensed under the Mozilla Public License 2.0 (MPL-2.0).
See the LICENSE file for details.
