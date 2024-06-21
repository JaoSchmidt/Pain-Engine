## Development
### Windows build

`cmake` is required

`nasm` is required, can be found (here)[https://www.nasm.us/]

`Straberrt Perl` is required, can be found (here)[https://strawberryperl.com/]

### Linux build

```
git clone --recursive https://github.com/JaoSchmidt/Pain-Engine
cd Pain-Engine
cmake -S ./ -B build/
cd build && make -j6
```
