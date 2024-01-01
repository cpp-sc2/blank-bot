# blank-bot

[![Build](https://github.com/cpp-sc2/blank-bot/actions/workflows/ci.yml/badge.svg)](https://github.com/cpp-sc2/blank-bot/actions/workflows/ci.yml)

## About
Starter bot for StarCraft II with integrated cpp-sc2.

## Support
Feel free to post questions on the unofficial StarCraft II AI Discord server. [Invite Link](https://discordapp.com/invite/Emm5Ztz)

## Build requirements
1. Download the [actual map pack](https://aiarena.net/wiki/maps/).

2. Put the downloaded maps into the Maps folder (create it if the folder doesn't exist):
  * Windows: C:\Program Files\StarCraft II\Maps
  * OS X: /Applications/StarCraft II/Maps
  * Linux: anywhere.

3. Download and install [CMake](https://cmake.org/download/).

4. A compiler with C++17 support.

5. Windows: Download and install Visual Studio ([2017](https://www.visualstudio.com/downloads/) or newer).

6. Linux: Install 'gcc-c++'.

7. Linux: Install the 'make' utility.

8. OS X: Install XCode.

## Build instructions

### Windows (Visual Studio)
```bat
:: Get the project.
$ git clone git@github.com:cpp-sc2/blank-bot.git
$ cd blank-bot

:: Create Visual Studio project files in the directory "build".
:: For Visual Studio 2022
$ cmake -B build -G "Visual Studio 17 2022"

:: Build the project using Visual Studio.
$ start build\BlankBot.sln

:: Launch the bot with the specified path to a SC2 map, e.g.
$ build\bin\Debug\BlankBot.exe Ladder2019Season3/AcropolisLE.SC2Map
```

### Linux (command line)
```bash
# Get the project.
$ git clone git@github.com:cpp-sc2/blank-bot.git && cd blank-bot

# Generate CMake build tree.
$ cmake -B build

# Build the project.
$ cmake --build build --parallel --verbose

# Launch the bot with the specified absolute path to a SC2 map, e.g.
$ ./build/bin/BlankBot "/home/alkurbatov/Ladder2019Season3/AcropolisLE.SC2Map"
```

### OS X (Xcode)
```bash
# Get the project.
$ git clone git@github.com:cpp-sc2/blank-bot.git && cd blank-bot

# Generate CMake build tree.
$ cmake -B build -G Xcode

# Build the project using Xcode.
$ open build/BlankBot.xcodeproj

# Launch the bot with the specified path to a SC2 map, e.g.
$ ./build/bin/BlankBot "Ladder2019Season3/AcropolisLE.SC2Map"
```

## Additional options

### WSL2 Support

Cross compiling for Windows under WSL2 is supported through `cpp-sc2`. See the `cpp-sc2` [documentation](https://github.com/cpp-sc2/cpp-sc2/blob/master/docs/building.md#wsl2-support) for build requirements. The build flag remains the same, setting `-DWSL_CROSS_COMPILE=ON`.

### Game client version
By default, the API assumes the latest version of the game client. The assumed version can be found in cmake's output, e.g.:
```bash
$ cmake -B build grep 'SC2 version'
Target SC2 version: 5.0.5
...
```

However, sometimes one may need to compile with an older version of the game, e.g. to play with a Linux build which is
always behind the Windows version. It is possible by specifying the game version manually, e.g.:
```bash
$ cmake -B build -DSC2_VERSION=4.10.0
```

### AIArena ladder build
To compile a bot capable to play on [the AIArena ladder](https://aiarena.net), configure the project in the following way:
```bash
$ cmake -B build -DBUILD_FOR_LADDER=ON -DSC2_VERSION=4.10.0
```

## Managing CMake dependencies

`BlankBot` uses the CMake `FetchContent` module to manage and collect dependencies. To use a version of `cpp-sc2` outside of the pinned commit, modify the `GIT_REPOSITORY` and/or the `GIT_TAG` in `cmake/cpp_sc2.cmake`:
```
...
FetchContent_Declare(
    cpp_sc2
    GIT_REPOSITORY <target-cpp-sc2-git-rep>
    GIT_TAG <git-commit-hash>
)
FetchContent_MakeAvailable(cpp_sc2)
...
```
With the updated configuration, re-run the build configuration and the dependency will be updated to the new configuration:
```
$ cmake -B build
```

## Troubleshooting
If you see that some of cmake options don't take effect
(e.g. project was configured with `cmake -B build -DBUILD_FOR_LADDER=ON` and a bit later with `cmake -B build`)
remove the `build` folder or the `CMakeCache.txt` file, or explicitly re-specify the desired default behavior (e.g. `cmake -B build -DBUILD_FOR_LADDER=OFF`).

## License
Copyright (c) 2021-2024 Alexander Kurbatov

Licensed under the [MIT license](LICENSE).
