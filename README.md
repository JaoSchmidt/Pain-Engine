## Development
### Windows build

`cmake` is required

`nasm` is required, can be found (here)[https://www.nasm.us/]

`Straberrt Perl` is required, can be found (here)[https://strawberryperl.com/]

---

### Linux build

You can clone directly from master:

```bash
git clone https://github.com/JaoSchmidt/Pain-Engine
cd Pain-Engine
```

Clone all essential submodules:

```bash
git submodule update --init --recursive \
    Pain/external/glm \
    Pain/external/spdlog \
    Pain/external/SDL_image \
    Pain/external/SDL \
    Pain/external/imgui \
    Pain/external/freetype \
    Pain/external/msdf-atlas-gen
```

Choose the correct game you want to compile, or create a custom path for your game/simulation.

To make that, edit `CMakeLists.txt` and search for GAME_FOLDER variable:

```
set(GAME_FOLDER "your/game/folder/here")
```

Then, create the `build` folder and compile:

``` bash
./configure.sh
cd build && nice make -j6
```
