# PicoSystem 32blit Boilerplate <!-- omit in toc -->

![Build](https://github.com/32blit/32blit-boilerplate/workflows/Build/badge.svg)

Implementing the game Space Invaders on PicoSystem.

It's based on the original `template` project from the 
[32blit beta](https://github.com/pimoroni/32blit-beta).


## Building

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

## Copying your game to your PicoSystem

Connect your PicoSystem to your computer using a USB Type-C cable.

From a power-off state, hold down X (the top face button) and press Power (the button at the top left, next to the USB Type-C port).

Your PicoSystem should mount as "RPI-RP2". On Linux this might be `/media/<username>/RPI-RP2`:

```
cp your-project-name.uf2 /media/`whoami`/RPI-RP2
```

The file should copy over, and your PicoSystem should automatically reboot into your game.

