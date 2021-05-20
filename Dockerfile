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
  python3 \
  ruby

ENV PRK_HOME /prk_firmware

VOLUME "${PRK_HOME}"
WORKDIR "${PRK_HOME}"
COPY . .

WORKDIR "${PRK_HOME}/src/ruby"
RUN bundle install

WORKDIR "${PRK_HOME}"
RUN git clone https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH "${PRK_HOME}/pico-sdk"
WORKDIR "${PICO_SDK_PATH}"
RUN git submodule update --init

WORKDIR "${PRK_HOME}"
ARG KEYBOARD

WORKDIR "${PRK_HOME}/keyboards"

WORKDIR "${PRK_HOME}/keyboards/${KEYBOARD}/build"

RUN cmake ../../..
RUN make

FROM scratch AS export

ENV PRK_HOME /prk_firmware
COPY --from=build "${PRK_HOME}/keyboards/${KEYBOARD}" "./${KEYBOARD}"

# ex. git clone https://github.com/picoruby/prk_meishi2.git keyboards/prk_meishi2 &&
#     docker build -o keyboards --build-arg KEYBOARD=prk_meishi2 .
# ex. git clone https://github.com/picoruby/prk_crkbd.git keyboards/prk_crkbd &&
#     docker build -o keyboards --build-arg KEYBOARD=prk_crkbd .
# ex. git clone https://github.com/picoruby/prk_claw44.git keyboards/prk_claw44 &&
#     docker build -o keyboards --build-arg KEYBOARD=prk_claw44 .
