# ManiaDrive

Open-source racing game built on the Raydium 3D engine.

**Fork features:**
- **Big-endian** support (PowerPC Linux, preliminary macOS/PPC)
- **CMake** build system
- **Bundled PHP 5.3** — patched for PowerPC64 ELFv2 visibility quirks (missing POSIX symbols, `language_scanner_globals`, `zendparse`)
- **Bundled ODE** — single precision forced, no system dependency
- **Kids Mode** (`-DKIDS_MODE=ON`) — unlock all tracks, show speed/accel sliders


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
- X11, Xinerama (not needed on macOS)
- v4l-utils (libv4lconvert, not needed on macOS)
- bison

**Required:**
- PHP 5.3 (bundled at `external/php-5.3.27`) — needed for story mode and online features (scores, track sharing).

Debian/Ubuntu:
```sh
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev \
  libglew-dev libopenal-dev libalut-dev libjpeg-dev libpng-dev zlib1g-dev \
  libcurl4-openssl-dev libxml2-dev libvorbis-dev libogg-dev \
  libx11-dev libxinerama-dev libv4l-dev bison
```

Arch Linux:
```sh
sudo pacman -S base-devel cmake mesa glu glew openal freealut libjpeg-turbo \
  libpng zlib curl libxml2 libvorbis libogg libx11 libxinerama v4l-utils \
  bison
```

Fedora:
```sh
sudo dnf install gcc gcc-c++ cmake mesa-libGL-devel mesa-libGLU-devel \
  glew-devel openal-soft-devel freealut-devel libjpeg-turbo-devel libpng-devel \
  zlib-devel libcurl-devel libxml2-devel libvorbis-devel libogg-devel \
  libX11-devel libXinerama-devel libv4l-devel bison
```

### macOS

Requires Xcode (or Command Line Tools) and [Homebrew](https://brew.sh):

```sh
brew install cmake glew freealut libjpeg libpng curl libxml2 libvorbis libogg bison
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For an `.app` bundle: `cmake -B build -DMANIADRIVE_MACOS_BUNDLE=ON`.

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
| `MANIADRIVE_MACOS_BUNDLE` | `OFF` | Build macOS .app bundle (macOS only) |

### Running

```sh
# From the build directory:
./mania_drive

# Or install system-wide:
sudo cmake --install .
```

The game expects data files (`rayphp/`, textures, sounds, etc.) relative to the working directory. Run from the project root or install first.

PHP scripts at the root of the repo (`anim.php`, `mania_localtracks.php`, etc.) must be copied or symlinked into the game's data directory:

```sh
mkdir -p ~/.mania_drive/data
cp *.php ~/.mania_drive/data/
# or symlink:
# ln -s "$PWD" ~/.mania_drive/data/.
```
### Cross-compilation

ODE is fetched from upstream (0.16.x) which supports arm64, ppc64le, etc. natively. PHP 5.3 may need additional configure flags for the target architecture — pass them via `CMAKE_C_FLAGS` and `PHP5_CONFIGURE_OPTS` if needed.

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

See the [LICENSE](LICENSE) file for details.
