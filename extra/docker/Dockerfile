FROM ubuntu:18.04

RUN apt-get update && apt-get install --no-install-recommends -y build-essential libphysfs-dev libboost-dev cmake libsdl2-dev libsdl2-image-dev \
libsdl2-gfx-dev libsdl2-mixer-dev libsdl2-ttf-dev libboost-program-options-dev libutfcpp-dev libbox2d-dev zip gettext

RUN mkdir -p /staging/saland

COPY . /staging/saland

RUN cd /staging/saland && \
cmake . && \
make
