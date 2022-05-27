# Document:
# https://github.com/picoruby/prk_firmware/wiki/Docker

FROM --platform=amd64 ruby:3.1.2-slim AS build

RUN apt-get update && apt-get install --no-install-recommends -y \
  cmake \
  gcc-arm-none-eabi \
  libnewlib-arm-none-eabi \
  libstdc++-arm-none-eabi-newlib \
  ca-certificates \
  git \
  gcc \
  make \
  g++ \
  zip \
  python3

RUN git clone --recursive https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH "/pico-sdk"

ENV PRK_HOME /prk_firmware
VOLUME "${PRK_HOME}"
WORKDIR "${PRK_HOME}"
COPY . .
RUN rake setup
