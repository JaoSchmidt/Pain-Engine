@echo off
cmake -DSDL2IMAGE_VENDORED=OFF -G "Visual Studio 16 2019" -S . -B ./build
