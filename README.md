# blank-bot

## About
Starter bot for StarCraft II with integrated cpp-sc2.

## Support
Feel free to post questions on the unofficial Starcraft II AI Discord server. [Invite Link](https://discordapp.com/invite/Emm5Ztz)

## Build requirements
1. Download (the password is iagreetotheeula) at least one of the following map packs:
  * [Ladder 2019 Season 1](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2019Season1.zip)
  * [Ladder 2019 Season 2](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2019Season2.zip)
  * [Ladder 2019 Season 3](http://blzdistsc2-a.akamaihd.net/MapPacks/Ladder2019Season3.zip)

2. Put the downloaded maps into the Maps folder (create it if the folder doesn't exist):
  * Windows: C:\Program Files\StarCraft II\Maps
  * OS X: /Applications/StarCraft II/Maps
  * Linux: anywhere.

3. Download and install [CMake](https://cmake.org/download/).

4. A compiler with C++14 support.

5. Windows: Download and install Visual Studio ([2019](https://www.visualstudio.com/downloads/) or older).

6. Linux: Install 'gcc-c++'.

7. Linux: Install the 'make' utility.

8. OS X: Install XCode.

## Build instructions

### Windows (Visual Studio)
```bat
:: Get the project.
$ git clone --recursive git@github.com:cpp-sc2/blank-bot.git
$ cd blank-bot

:: Create build directory.
$ mkdir build
$ cd build

:: Create Visual Studio project files.
:: For Visual Studio 2019
$ cmake ../ -G "Visual Studio 16 2019"
:: For Visual Studio 2017
$ cmake ../ -G "Visual Studio 15 2017 Win64"

:: Build the project using Visual Studio.
$ start BlankBot.sln

:: Launch the bot with the specified path to a SC2 map, e.g.
$ bin\Debug\BlankBot.exe Ladder2019Season3/AcropolisLE.SC2Map
```

### Linux (command line)
```bash
# Get the project.
$ git clone --recursive git@github.com:cpp-sc2/blank-bot.git && cd blank-bot

# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
$ cmake ../

# Build the project.
$ VERBOSE=1 cmake --build . --parallel

# Launch the bot with the specified absolute path to a SC2 map, e.g.
$ ./bin/BlankBot "/home/alkurbatov/Ladder2019Season3/AcropolisLE.SC2Map"
```

### OS X (Xcode)
```bash
# Get the project.
$ git clone --recursive git@github.com:cpp-sc2/blank-bot.git && cd blank-bot

# Create build directory.
$ mkdir build && cd build

# Generate a Makefile.
# Use 'cmake -DCMAKE_BUILD_TYPE=Debug ../' if debuginfo is needed
$ cmake ../ -G Xcode

# Build the project using Xcode.
$ open BlankBot.xcodeproj

# Launch the bot with the specified path to a SC2 map, e.g.
$ ./bin/BlankBot "Ladder2019Season3/AcropolisLE.SC2Map"
```

## Additional options

### Game client version
By default, the API assumes the latest version of the game client. The assumed version can be found in cmake's output, e.g.:
```bash
$ cmake ../ | grep 'SC2 version'
Target SC2 version: 5.0.5
...
```

However, sometimes one may need to compile with an older version of the game, e.g. to play with a Linux build which is
always behind the Windows version. It is possible by specifying the game version manually, e.g.:
```bash
$ cmake -DSC2_VERSION=4.10.0 ../
```

## License
Copyright (c) 2021 Alexander Kurbatov

Licensed under the [MIT license](LICENSE).
