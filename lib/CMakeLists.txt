cmake_minimum_required(VERSION 3.13)
include(../pico_sdk_import.cmake)
project(picoruby
  VERSION 0.0.1
  DESCRIPTION "mruby/c VM implementation"
  LANGUAGES C
)

execute_process (
  COMMAND ln -sf ../../../../../../lib/hal/hal.c
  COMMAND ln -sf ../../../../../../lib/hal/hal.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/src/mrubyc/src/hal_user_reserved
)

add_library(picoruby STATIC
  ./picoruby/src/mrubyc/src/alloc.c
  ./picoruby/src/mrubyc/src/c_array.c
  ./picoruby/src/mrubyc/src/c_hash.c
  ./picoruby/src/mrubyc/src/c_math.c
  ./picoruby/src/mrubyc/src/c_numeric.c
  ./picoruby/src/mrubyc/src/c_object.c
  ./picoruby/src/mrubyc/src/c_range.c
  ./picoruby/src/mrubyc/src/c_string.c
  ./picoruby/src/mrubyc/src/class.c
  ./picoruby/src/mrubyc/src/console.c
  ./picoruby/src/mrubyc/src/error.c
  ./picoruby/src/mrubyc/src/global.c
  ./picoruby/src/mrubyc/src/keyvalue.c
  ./picoruby/src/mrubyc/src/load.c
  ./picoruby/src/mrubyc/src/mrblib.c
  ./picoruby/src/mrubyc/src/rrt0.c
  ./picoruby/src/mrubyc/src/symbol.c
  ./picoruby/src/mrubyc/src/value.c
  ./picoruby/src/mrubyc/src/vm.c
  ./picoruby/src/mrubyc/src/hal_user_reserved/hal.c

  ./picoruby/src/common.c
  ./picoruby/src/compiler.c
  ./picoruby/src/dump.c
  ./picoruby/src/generator.c
  ./picoruby/src/regex_light/src/regex.c
  ./picoruby/src/my_regex.c
  ./picoruby/src/node.c
  ./picoruby/src/scope.c
  ./picoruby/src/stream.c
  ./picoruby/src/token.c
  ./picoruby/src/tokenizer.c
)

add_custom_target(picorbc
  COMMAND make picorbc DIR=host-production/alloc_libc LIBS=-lpicorbc CFLAGS=-DMRBC_USE_HAL_POSIX LDFLAGS=-L${CMAKE_CURRENT_SOURCE_DIR}/picoruby/build/lib/host-production/alloc_libc
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/cli
)
add_custom_target(mrblib
  COMMAND make MRBC=${CMAKE_CURRENT_SOURCE_DIR}/picoruby/build/bin/host-production/alloc_libc/picorbc
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/src/mrubyc/mrblib
)
add_custom_target(ptr_size
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/src/include
)
add_custom_target(tokenizer_helper
  COMMAND make tokenizer_helper.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/picoruby/src
)
add_dependencies(picoruby mrblib)
add_dependencies(mrblib picorbc ptr_size tokenizer_helper)

target_link_libraries(picoruby pico_stdlib)
target_compile_features(picoruby PRIVATE)
target_include_directories(picoruby INTERFACE
  ./picoruby/src
  ./picoruby/src/ruby-lemon-parse
  ./picoruby/src/mrubyc/src
  ./picoruby/src/mrubyc/src/hal_rp2040
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
