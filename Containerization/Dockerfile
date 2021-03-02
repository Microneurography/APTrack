FROM ubuntu:focal

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends\
    g++-8 \
    make \
    cmake \
    git ca-certificates \
    freeglut3-dev libfreetype6-dev libxinerama-dev libxcursor-dev libasound2-dev libxrandr-dev libcurl4-gnutls-dev \
 && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 -b master https://github.com/open-ephys/plugin-GUI.git

WORKDIR /plugin-GUI/Build

RUN export CC=gcc-8 CXX=g++-8 \
  && cmake .. \
  && make

WORKDIR /
