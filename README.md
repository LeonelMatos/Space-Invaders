# Space Invaders

![Build](https://github.com/32blit/32blit-boilerplate/workflows/Build/badge.svg)

Implementing the game Space Invaders on PicoSystem.

It's based on the original `template` project from the 
[32blit beta](https://github.com/pimoroni/32blit-beta).


## Building

### Requirements

Make sure you have *cmake* installed:

```bash
sudo apt install cmake
```

Also you need to have installed `32blit-sdk` and `pico-sdk` to build for the *PicoSystem*.

Follow *[32blit's README documentation](https://github.com/32blit/32blit-sdk?tab=readme-ov-file)* for additional guiding.

### PicoSystem

If you've got local copies of the Pico SDK, Pico Extras and 32blit SDK alongside your project,
then you can configure and build your .uf2 like so:

```
mkdir build.pico
cd build.pico
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico.toolchain -DPICO_BOARD=pimoroni_picosystem
```

If you'd like the Pico SDK to handle grabbing Pico SDK and Pico Extras for you, you can use:

```
mkdir build.pico
cd build.pico
cmake .. -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico.toolchain -DPICO_BOARD=pimoroni_picosystem -DPICO_SDK_FETCH_FROM_GIT=true -DPICO_EXTRAS_FETCH_FROM_GIT=true ..
```

:warning: Note: This approach is not recommended, since you might be reconfiguring a few times during
your project and re-downloading things unecessarily!

The one I use is:

```bash
cmake .. \
  -DCMAKE_TOOLCHAIN_FILE=../../32blit-sdk/pico.toolchain \
  -DPICO_BOARD=pimoroni_picosystem \
  -DPLATFORM=pico
```

Make sure you have pico-sdk installed on the parent folder and in it do:

```bash
git submodule update --init
```

### Linux

Build into the folder `build` 
```
cmake -B build -S . -DPLATFORM=linux
cd build
make
```

### Windows

Building to Windows while staying in the Linux environment means you need *MinGW-w64* installed.

```bash
sudo apt install mingw-w64
```

*Create a Windows build directory*
```bash
mkdir build.win && cd build.win
```

*Invoke cmake for cross-compiling*
```bash
cmake .. \
  -G "Unix Makefiles" \
  -DPLATFORM=host \
  -D32BLIT_DIR=/home/leonel/32blit/32blit-sdk \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
```

*Build*
```bash
make
```

Then you can run the `zip-win-build.sh` script to zip all the game files.

## Copying your game to your PicoSystem

Connect your PicoSystem to your computer using a USB Type-C cable.

From a power-off state, hold down X (the top face button) and press Power (the button at the top left, next to the USB Type-C port).

Your PicoSystem should mount as "RPI-RP2". On Linux this might be `/media/<username>/RPI-RP2`:

```
cp your-project-name.uf2 /media/`whoami`/RPI-RP2
```

The file should copy over, and your PicoSystem should automatically reboot into your game.

## Notes

- The game textures were made to follow the PicoSystem's OCS palette of 4096 colors. For this I used the plugin
[Amiga OCS/ECS Color Palette Mixer](https://prismaticrealms.itch.io/aseprite-script-amiga-ocsecs-color-palette-mixer) made by *prismaticrealms* for Aseprite.

## Author

- [@leonelmatos](https://www.github.com/LeonelMatos)
