# Building
cmake and git are assumed to be installed already.
All submodules should be updated with
```
git submodule update --init --recursive
```

## To build in windows
* Install [MinGW](https://www.mingw-w64.org/)
* vscode with the recommended extensions

```
code .
```
use ctrl-B

## To build in linux

```
mkdir build
cd build
cmake ..
cmake --build
```

## Pretty printing 
Pretty printers for gdb for Logic types are included in prettyprint.py. These are loaded with a setupCommands when debugging the console or test mate tests from vscode.