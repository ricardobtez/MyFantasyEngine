# My Fantasy Engine

Pre-rendered background game engine
 

# How to build 

After cloning the repository, download the submodules by 

```bash
git submodule init
git submodule update
```

## Linux
To build on linux, you must have the following dependencies installed
- clang-7 or higher
- libglu1-mesa-dev
- libc++-7-dev (libcxx-devel and libcxx-static on Fedora) or higher
- libc++abi-7-dev (libcxxabi-static on Fedora) or higher
- ninja-build
- libxi-dev
- libxxf86vm-dev

After installing the dependencies, following the following steps 
```bash
> mkdir build && cd build 
> CC=/usr/bin/clang CXX=/usr/bin/clang++ CXXFLAGS=-stdlib=libc++ \
    cmake -G Ninja -DCMAKE_BUILD_TYPE=<Mode> ..
> ninja
```
You can also use `make` instead of Ninja and \<Mode\> can either be `Release` or `Debug`.

## Windows
The following dependencies are a prerequisite of building this project on Windows
- Visual Studio 2019 or later
- Windows 10 SDK
- Python 3.7 or higher
- CMake 3.14 or later

After obtaining them, follow these steps 
```bash
> mkdir build 
> cd build 
> cmake ..
> cmake --build . --target MyFantasyEngine --config <Mode>
```

`MyFantasyEngine.exe` will be compiled to the 	`bin` folder
