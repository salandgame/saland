# Saland Adventures - The game

![Early screen shot from the game](https://files.poulsander.com/~poul19/public_files/saland_2019-04-10_21-03-21.png)

The game is a 2d top down RPG. One of the special things is that you can either change in the world using Tiled or do changes to in game.

The game is pre alpha.

For more info the [homepage](https://salandgame.github.io/)

## Controls
WASD = Move

1-9,0 = Choose spell

Click = Use magic

# Building
The only supported build method is using the CMake<br/>
To build do:
```
./packdata.sh
cmake .
# or
# cmake -DCMAKE_BUILD_TYPE=Debug .
# or
# cmake -DCMAKE_BUILD_TYPE=Release .
make
```
The result should be in the build-folder. To run
```
./saland
```

## Building using Docker

As getting a C++ project with many dependencies to compile can be a daunting task then I have provided a couple of Docker images that can perform a build. Both for Windows and Linux.

On a fresh checkout you can use:
```
docker build . -f extra/docker/Dockerfile -t saland_test
```
and
```
docker build . -f extra/docker/Dockerfile.WindowsBuild -t saland_test
```

The Docker files gives a step by step guide to create a build environment.


## Downloads
The game is unreleased but Windows nightly builds can be found here:
https://files.poulsander.com/~poul19/public_files/saland/windows_nightly/
