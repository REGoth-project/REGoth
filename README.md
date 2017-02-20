# REGoth [![Build status](https://ci.appveyor.com/api/projects/status/50yputp2qe764sg2/branch/master?svg=true)](https://ci.appveyor.com/project/degenerated1123/regoth) [![Build Status](https://travis-ci.org/REGoth-project/REGoth.svg?branch=master)](https://travis-ci.org/REGoth-project/REGoth) [![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/REGoth/Lobby)
OpenSource-Reimplementation of the zEngine, used by the game "Gothic" and "Gothic II".

Don't forget to check out the [REGoth-Wiki](../../wiki) for more information about the project!

A list showing the current state of the engine, can be found [here](../../wiki/state).

### Videos showing the current state
* Release-Commentary to V0.3:
   * https://www.youtube.com/watch?v=EjEpN3I8Ovc
* Release-Commentary to V0.2:
   * https://www.youtube.com/watch?v=jB3pzxd6vIk
* Release-Commentary to V0.1.1:
   * https://www.youtube.com/watch?v=heg49o78Ii4
* Dialogs, Audio, NPC script-states:
   * https://www.youtube.com/watch?v=R3F4WY6A060
* Day/Night-Cycle and Fighting-animations: 
   * https://www.youtube.com/watch?v=Z1mlI9qxkzc
* First showcase: 
   * https://www.youtube.com/watch?v=8bLAGttYYpY

# Download (Latest stable version)

**Version 0.3**: https://github.com/REGoth-project/REGoth/releases/tag/0.3

Automated nightly builds: https://github.com/degenerated1123/REGoth/releases

# Source
Make sure to clone this repository with the `--recursive` flag:

##### ssh:
```sh
git clone --recursive git@github.com:degenerated1123/REGoth.git
```

##### https:
```sh
git clone --recursive https://github.com/degenerated1123/REGoth.git
```

To update the repo, you need to make sure to update the submodules as well. Instead of simply pulling the repository, you can do:
```sh
git pull --recurse-submodules
```

# Building

> Note: If you are missing packages or are having trouble building on your platform, you can check out this [wiki-page](../../wiki/how-to-build), which may contain more detailed instructions.
> If you don't find your platform there, I'd kindly ask you to add some instructions for other people to that wiki-page!

You will need CMake (3.1 or newer) and a C++14-capable compiler.
Currently supported/tested are:
 - GCC (6.x)
 - MinGW
 - Visual Studio 2015

Then:
```sh
cd path/to/REGoth
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```
> If CMake complains about some folders missing a CMakeLists.txt, you most likely forgot to clone with the `--recursive`-flag!
> Simple do `git submodule update --init --recursive` and it should work.


Then, choose depending on your OS:

### Linux
```sh
make -j4
```

### Windows
Open the generated `REGoth.sln` in VisualStudio and build as usual.

Alternatively, you may do this (inside the build-folder):
```sh
cmake --build . --config release
``` 



# Running
The compiled files are inside the `build/bin` directory.

Make sure to copy the `content/shaders` folder to the working directory of the compiled `REGoth`-Executable.
Then, run the program with the following flags:
```sh
REGoth -g "path/to/gothic1or2" -w startworld.zen
```

Where `path/to/gothic1or2` points to the root of a Gothic I or II installation and `startworld.zen` is one of the Zen-Files found in a `.vdf`-files in the games `data/`-directory. For example: `newworld.zen` or `addonworld.zen` for Gothic II.

> It is recommended to run this from the commandline, to see the debug-output of the program.

Additionally, for a list of possible commands, run `REGoth --help`.

# Controls

 * Movement: WASD/QE or arrow-keys (Space or Shift to run faster)
 * Actions: Left CTRL for everything
 * Menus: B for status-screen
 * Console: F10
   * Available commands:
      * `kill`: Kill a nearby NPC
      * `knockout`: Knockout a nearby NPC
      * `save <name>`: Save the game
      * `load <name>`: Load a game
      * `switchlevel <zenfile>`: Switch to an other level in current session
      * `usemana <amount>`: Use mana
      * `hurtself <amount>`: Hurt yourself
      * `timeset <x>`: Set time to 12:00. `x` doesn't matter but must be a number.
      * `list`: Print a list of available commands
      
# Development

If you want to help out and don't know where to start, I suggest reading the [wiki-page](../../wiki), which contains information about the engine-layout and lists of which features are missing (Not yet, though!). 

### Examples
**Gothic 1 - Overworld:** `REGoth -g "path/to/gothic1" -w world.zen`

**Gothic 1 - Oldmine:** `REGoth -g "path/to/gothic1" -w oldmine.zen`

**Gothic 1 - Freemine:** `REGoth -g "path/to/gothic1" -w freemine.zen`

**Gothic 1 - Sleeper temple:** `REGoth -g "path/to/gothic1" -w orctemple.zen`

**Gothic 2 - Overworld:** `REGoth -g "path/to/gothic2" -w newworld.zen`

**Gothic 2 - Valley of mines:** `REGoth -g "path/to/gothic2" -w oldworld.zen`

**Gothic 2 - Addonworld:** `REGoth -g "path/to/gothic2" -w addonworld.zen`

**Gothic 2 - Dragonisland:** `REGoth -g "path/to/gothic2" -w dragonisland.zen`
