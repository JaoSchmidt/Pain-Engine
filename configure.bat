@echo off
cmake -G "Visual Studio 16 2019" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B ./build
