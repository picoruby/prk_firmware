# Document:
# https://github.com/picoruby/prk_firmware/wiki/Docker

FROM --platform=amd64 manjarolinux/base AS build

RUN pacman -Sy --noconfirm \
  cmake \
  arm-none-eabi-gcc \
  arm-none-eabi-newlib \
  ca-certificates \
  git \
  gcc \
  make \
  zip \
  python \
  ruby \
  ruby-rake \
  ruby-bundler

RUN git clone https://github.com/raspberrypi/pico-sdk.git -b 1.5.1
RUN cd /pico-sdk/lib && git submodule update --init ./
ENV PICO_SDK_PATH "/pico-sdk"

ENV PRK_HOME /prk_firmware
VOLUME "${PRK_HOME}"
WORKDIR "${PRK_HOME}"
COPY . .
RUN rake setup
RUN git config --global --add safe.directory /prk_firmware

