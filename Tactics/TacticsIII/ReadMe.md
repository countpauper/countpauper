# Buildimg
cmake and git are assumed to be installed already.
All submodules should be updated with
```
git submodule update --init --recursive
```

## To build in windows
* Install [MinGW](https://www.mingw-w64.org/)
* Install FreeGLUT from MinGW with `pacman -S mingw-w64-ucrt-x86_64-freeglut`
* Install GLEW with `pacman -S mingw-w64-ucrt-x86_64-glew`
* vscode with the recommended extensions

```
set VCPKG_ROOT=C:\...\Dependencies\cpkg
set PATH=%VCPKG_ROOT%;%PATH%
cd Tactics\TacticsIII
# vcpkg stuff to add opengl?
cd ..
code III.code-workspace
```

## To build in linux

```
sudo apt-get install freeglut3-dev
sudo apt-get install libglew-dev
mkdir build
cd build
cmake ..
cmake --build
```
