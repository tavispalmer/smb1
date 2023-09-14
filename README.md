# smb1

A (work-in-progress) C port of Super Mario Bros. for the NES.

## Building

CMake is used for building the executable.

### Linux

`glfw` and `openal` are linked dynamically, so they must first be installed with your package manager.

With Ninja:

```
$ cmake -B build -G Ninja
$ ninja -C build
```

With Make:

```
$ cmake -B build -G 'Unix Makefiles'
$ make -C build
```

The resulting executable should be located at `build/smb1`.

### Windows

With MSVC:

```
$ cmake -B build -G 'Visual Studio 17 2022'
$ cmake --build build
```

The resulting executable should be located at either `build/Debug/smb1.exe` or `build/Release/smb1.exe`.

With MinGW:

```
$ cmake -B build -G 'MinGW Makefiles'
$ mingw32-make -C build
```

The resulting executable should be located at `build/smb1.exe`.
