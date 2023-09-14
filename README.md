# smb1

A (work-in-progress) C port of Super Mario Bros. for the NES.

## Building

Make sure CMake is installed on your system, as it is used for building the executable.

In order to successfully build, **a copy of the original Super Mario Bros. ROM is required.**
Rename the ROM file `smb1.nes`, and place it in the root directory of this repository *before* running the commands below.

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
