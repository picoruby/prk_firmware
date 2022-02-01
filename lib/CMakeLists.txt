cmake_minimum_required(VERSION 3.13)
include(../pico_sdk_import.cmake)
project(picoruby
  VERSION 0.0.1
  DESCRIPTION "PicoRuby"
  LANGUAGES C
)

set(MRUBYC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/build/repos/host/mruby-mrubyc/repos/mrubyc)
set(PICO_COMPILER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/build/repos/host/mruby-pico-compiler)

add_library(picoruby STATIC
  ${MRUBYC_DIR}/src/alloc.c
  ${MRUBYC_DIR}/src/c_array.c
  ${MRUBYC_DIR}/src/c_hash.c
  ${MRUBYC_DIR}/src/c_math.c
  ${MRUBYC_DIR}/src/c_numeric.c
  ${MRUBYC_DIR}/src/c_object.c
  ${MRUBYC_DIR}/src/c_range.c
  ${MRUBYC_DIR}/src/c_string.c
  ${MRUBYC_DIR}/src/class.c
  ${MRUBYC_DIR}/src/console.c
  ${MRUBYC_DIR}/src/error.c
  ${MRUBYC_DIR}/src/global.c
  ${MRUBYC_DIR}/src/keyvalue.c
  ${MRUBYC_DIR}/src/load.c
  ${MRUBYC_DIR}/src/mrblib.c
  ${MRUBYC_DIR}/src/rrt0.c
  ${MRUBYC_DIR}/src/symbol.c
  ${MRUBYC_DIR}/src/value.c
  ${MRUBYC_DIR}/src/vm.c
  ${MRUBYC_DIR}/src/hal_user_reserved/hal.c
  ${PICO_COMPILER_DIR}/src/common.c
  ${PICO_COMPILER_DIR}/src/compiler.c
  ${PICO_COMPILER_DIR}/src/dump.c
  ${PICO_COMPILER_DIR}/src/generator.c
  ${PICO_COMPILER_DIR}/src/regex.c
  ${PICO_COMPILER_DIR}/src/my_regex.c
  ${PICO_COMPILER_DIR}/src/node.c
  ${PICO_COMPILER_DIR}/src/scope.c
  ${PICO_COMPILER_DIR}/src/stream.c
  ${PICO_COMPILER_DIR}/src/token.c
  ${PICO_COMPILER_DIR}/src/tokenizer.c
)

target_link_libraries(picoruby pico_stdlib)
target_compile_features(picoruby PRIVATE)
target_include_directories(picoruby PRIVATE
  ${MRUBYC_DIR}/src
  ${PICO_COMPILER_DIR}/include
)

# For hal.c
target_include_directories(picoruby PUBLIC
  ${PICO_SDK_PATH}/lib/tinyusb/src
  ../include
)

set_target_properties(picoruby
  PROPERTIES
  VERSION ${PROJECT_VERSION}
)
