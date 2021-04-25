FROM debian:10-slim AS build

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

RUN git clone https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH "${PRK_HOME}/pico-sdk"
WORKDIR "${PICO_SDK_PATH}"
RUN git submodule update --init

WORKDIR "${PRK_HOME}"
ARG REPO_URL
ARG REPO_NAME

WORKDIR "${PRK_HOME}/keyboards"

RUN git clone $REPO_URL
WORKDIR "${PRK_HOME}/keyboards/${REPO_NAME}/build"

RUN cmake ../../..
RUN make

FROM scratch AS export

ENV PRK_HOME /prk_firmware
COPY --from=build "${PRK_HOME}/keyboards/${REPO_NAME}" "./${REPO_NAME}"

# ex. docker build -o keyboards --build-arg REPO_NAME=prk_meishi2 --build-arg REPO_URL=https://github.com/picoruby/prk_meishi2.git .
# ex. docker build -o keyboards --build-arg REPO_NAME=prk_crkbd --build-arg REPO_URL=https://github.com/picoruby/prk_crkbd.git .
# ex. docker build -o keyboards --build-arg REPO_NAME=prk_claw44 --build-arg REPO_URL=https://github.com/picoruby/prk_claw44.git .

