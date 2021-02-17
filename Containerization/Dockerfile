FROM ubuntu:focal

RUN apt update \
  && apt -y install git \
  && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/open-ephys/plugin-GUI.git

WORKDIR /plugin-GUI/Resources/Scripts

RUN apt update \
  && ./install_linux_dependencies.sh \
  && apt -y install gcc-8 g++-8 \
  && DEBIAN_FRONTEND="noninteractive" apt -y install cmake \
  # && apt install -y udev \
  # && cp ./40-open-ephys.rules /etc/udev/rules.d \
  # && service udev restart \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /plugin-GUI/Build

RUN export CC=gcc-8 CXX=g++-8 \
  && cmake .. \
  && make

WORKDIR /
