FROM ubuntu:24.04

# Set timezone to UTC. This prevents apt-get install SOME-PACKAGES from asking for timezone during setup
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Note: SDL3 is not packaged on Ubuntu 22.04/24.04, so it's built from source via
# CMake FetchContent (see top-level CMakeLists.txt). The host therefore needs
# curl/ca-certificates plus the codec libraries that SDL3_image/SDL3_mixer/SDL3_ttf use.
RUN apt-get update && apt-get install --no-install-recommends -y \
    build-essential \
    cmake \
    curl \
    ca-certificates \
    pkg-config \
    libphysfs-dev \
    libboost-dev \
    libboost-program-options-dev \
    zlib1g-dev \
    libfreetype-dev \
    libpng-dev \
    libjpeg-dev \
    libogg-dev \
    libvorbis-dev \
    libasound2-dev \
    libpulse-dev \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxi-dev \
    libxcursor-dev \
    libxinerama-dev \
    libxss-dev \
    libxtst-dev \
    libwayland-dev \
    wayland-protocols \
    libxkbcommon-dev \
    libdrm-dev \
    libgbm-dev \
    libudev-dev \
    libdbus-1-dev \
    libibus-1.0-dev \
    libsamplerate0-dev \
    libusb-1.0-0-dev \
    libdecor-0-dev \
    libpipewire-0.3-dev \
    zip \
    gettext

RUN mkdir -p /staging/saland

COPY . /staging/saland

# Build saland out-of-source. SDL3 (and friends) is fetched and built from
# source via CMake's FetchContent. The .dockerignore keeps build/ and
# embedded_libs/downloads/ out of the build context so we always pull fresh
# tarballs over the network (curl + ca-certificates are installed above).
RUN cmake -S /staging/saland -B /staging/saland/build && \
    cmake --build /staging/saland/build -j"$(nproc)"
