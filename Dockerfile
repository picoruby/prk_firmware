# Usage:
# - Setup:
#   docker build . -r prk
# - Build:
#   docker run --rm -v $PWD:/prk_firmware prk rake
# - Clean built:
#   docker run --rm -v $PWD:/prk_firmware prk clean
# - Build with keymap:
#   docker run --rm -v $PWD:/prk_firmware prk rake build_with_keymap[prk_meishi2]
# - Clean built with keymap:
#   docker run --rm -v $PWD:/prk_firmware prk rake clean_with_keymap[prk_meishi2]

FROM ruby:3.0.1-slim AS build

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
  python3 \
  ruby

RUN git clone --recursive https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH "/pico-sdk"

ENV PRK_HOME /prk_firmware
VOLUME "${PRK_HOME}"
WORKDIR "${PRK_HOME}"
COPY . .
RUN rake setup
