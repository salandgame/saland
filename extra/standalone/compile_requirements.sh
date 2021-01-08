#! /bin/bash
set -euo pipefail

#https://github.com/erincatto/box2d/archive/v2.4.1.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://files.poulsander.com/~poul19/public_files/box2d-2.4.1.tar.gz | tar -zx && cd box2d-2.4.1 && ls -lrt
cd /staging/deps/box2d-2.4.1 && cmake . && make && make install

mkdir -p /staging/deps && cd /staging/deps && curl https://libsdl.org/release/SDL2-2.0.12.tar.gz | tar -zx && cd SDL2-2.0.12 && ls -lrt
cd /staging/deps/SDL2-2.0.12 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz | tar -zx && cd SDL2_image-2.0.5 && ls -lrt
cd /staging/deps/SDL2_image-2.0.5 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz | tar -zx && cd SDL2_mixer-2.0.4 && ls -lrt
cd /staging/deps/SDL2_mixer-2.0.4 && ./configure --enable-shared --enable-static && make && make install

#https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz | tar -zx && cd SDL2_ttf-2.0.15 && ls -lrt
cd /staging/deps/SDL2_ttf-2.0.15 && ./configure --enable-shared --enable-static && make && make install

#http://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.4.tar.gz
mkdir -p /staging/deps && cd /staging/deps && curl https://files.poulsander.com/~poul19/public_files/SDL2_gfx-1.0.4.tar.gz | tar -zx && cd SDL2_gfx-1.0.4 && ls -lrt
cd /staging/deps/SDL2_gfx-1.0.4 && ./configure --enable-shared --enable-static && make && make install

mkdir -p /staging/deps && cd /staging/deps && curl https://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2 | tar -jx && cd physfs-3.0.2 && ls -lrt
cd /staging/deps/physfs-3.0.2 && cmake . && make && make install

# boost
cd ~
mkdir -p Downloads
cd Downloads
curl https://files.poulsander.com/~poul19/public_files/boost_1_65_1.tar.bz2 -O
tar xvfj boost_1_65_1.tar.bz2
pushd boost_1_65_1
./bootstrap.sh --with-libraries=program_options
./b2 install -j 2 --prefix=/usr link=static
popd


mkdir -p /staging/saland-game