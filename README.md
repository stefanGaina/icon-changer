# icon-changer
*icon-changer* is a lightweight Windows x86_64 tool for changing an executable's icon.
# Usage
You can *download the latest released binary* or build from source like this:
- git clone https://github.com/stefanGaina/icon-changer.git
- cd icon-changer
- mkdir build
- cmake -G "Ninja" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -S . -B build
- cmake --build build

To execute run icon-changer path/to/icon.ico path/to/executable.exe.
The icon needs to be in a .ico format (images can be converted to this format).
