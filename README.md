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

Install dependencies:
```bash
# text, graphics driver, sound, etc
sudo apt-get install -y \
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
