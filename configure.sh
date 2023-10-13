#/bin/bash/
current_dir=$(pwd)
if [ "$(basename "$current_dir")" = "build" ]; then
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S ..
else
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B ./build
fi
