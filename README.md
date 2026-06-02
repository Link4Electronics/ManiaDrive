# ManiaDrive

Open-source racing game built on the Raydium 3D engine.

## Building

### Dependencies

**Required:**
- C/C++ compiler (GCC or Clang)
- CMake ≥ 3.16
- OpenGL, GLU, GLEW
- OpenAL + ALUT (freealut)
- libjpeg, libpng, zlib
- libcurl, libxml2
- libvorbis, libvorbisfile, libogg
- X11, Xinerama
- v4l-utils (libv4lconvert)

**Optional:**
- PHP 5.3 (bundled tarball at `raydium/php-latest.tar.gz`) — needed for online features (scores, track sharing). Disable with `-DPHP_SUPPORT=OFF`.

Debian/Ubuntu:
```sh
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev \
  libglew-dev libopenal-dev libalut-dev libjpeg-dev libpng-dev zlib1g-dev \
  libcurl4-openssl-dev libxml2-dev libvorbis-dev libogg-dev \
  libx11-dev libxinerama-dev libv4l-dev
```

Arch Linux:
```sh
sudo pacman -S base-devel cmake mesa glu glew openal freealut libjpeg-turbo \
  libpng zlib curl libxml2 libvorbis libogg libx11 libxinerama v4l-utils
```

Fedora:
```sh
sudo dnf install gcc gcc-c++ cmake mesa-libGL-devel mesa-libGLU-devel \
  glew-devel openal-soft-devel freealut-devel libjpeg-turbo-devel libpng-devel \
  zlib-devel libcurl-devel libxml2-devel libvorbis-devel libogg-devel \
  libX11-devel libXinerama-devel libv4l-devel
```

### Build

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

This produces two binaries in the build directory:
- `mania_drive` — the game
- `level_editor` — the track editor

### Kids Mode

Enable with `-DKIDS_MODE=ON`:

```sh
cmake .. -DKIDS_MODE=ON
```

Kids mode unlocks all tracks (no need to complete beginners mode first) and shows speed/acceleration sliders in the story mode GUI.

### Options

| Option | Default | Description |
|--------|---------|-------------|
| `KIDS_MODE` | `OFF` | Unlock all tracks, add speed/accel sliders |
| `PHP_SUPPORT` | `ON` | Enable PHP scripting (for online features) |

### Running

```sh
# From the build directory:
./mania_drive

# Or install system-wide:
sudo cmake --install .
```

The game expects data files (`rayphp/`, textures, sounds, etc.) relative to the working directory. Run from the project root or install first.

### Cross-compilation

ODE is fetched from upstream (0.16.x) which supports arm64, ppc64le, etc. natively. PHP 5.3 may need additional configure flags for the target architecture — pass them via `CMAKE_C_FLAGS` and `PHP5_CONFIGURE_OPTS` if needed.

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

See the [LICENSE](LICENSE) file for details.
