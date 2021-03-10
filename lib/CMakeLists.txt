cmake_minimum_required(VERSION 3.13)
include(../pico_sdk_import.cmake)
project(mmruby
  VERSION 0.0.1
  DESCRIPTION "mruby/c VM implementation"
  LANGUAGES C
)
add_definitions(-DMRBC_USE_HAL_RASPI_PICO -DMRBC_REQUIRE_32BIT_ALIGNMENT)
add_library(mmruby STATIC
  ./mmruby/src/mrubyc/src/alloc.c
  ./mmruby/src/mrubyc/src/c_array.c
  ./mmruby/src/mrubyc/src/c_hash.c
  ./mmruby/src/mrubyc/src/c_math.c
  ./mmruby/src/mrubyc/src/c_numeric.c
  ./mmruby/src/mrubyc/src/c_object.c
  ./mmruby/src/mrubyc/src/c_range.c
  ./mmruby/src/mrubyc/src/c_string.c
  ./mmruby/src/mrubyc/src/class.c
  ./mmruby/src/mrubyc/src/console.c
  ./mmruby/src/mrubyc/src/error.c
  ./mmruby/src/mrubyc/src/global.c
  ./mmruby/src/mrubyc/src/keyvalue.c
  ./mmruby/src/mrubyc/src/load.c
  ./mmruby/src/mrubyc/src/mrblib.c
  ./mmruby/src/mrubyc/src/rrt0.c
  ./mmruby/src/mrubyc/src/symbol.c
  ./mmruby/src/mrubyc/src/value.c
  ./mmruby/src/mrubyc/src/vm.c
  ./mmruby/src/mrubyc/src/hal_raspi_pico/hal.c

  ./mmruby/src/common.c
  ./mmruby/src/compiler.c
  ./mmruby/src/generator.c
  ./mmruby/src/my_regex.c
  ./mmruby/src/node.c
  ./mmruby/src/scope.c
  ./mmruby/src/stream.c
  ./mmruby/src/token.c
  ./mmruby/src/tokenizer.c

  ./mmruby/src/ruby-lemon-parse/crc.c
)

add_custom_target(ptr_size
  COMMAND make
  WORKING_DIRECTORY ../../lib/mmruby/src/include
)
add_custom_target(tokenizer_helper
  COMMAND make tokenizer_helper.h
  WORKING_DIRECTORY ../../lib/mmruby/src
)
add_custom_target(parse
  COMMAND make
  WORKING_DIRECTORY ../../lib/mmruby/src/ruby-lemon-parse
)
add_dependencies(mmruby parse ptr_size tokenizer_helper)

target_link_libraries(mmruby pico_stdlib)
target_compile_features(mmruby PRIVATE)
target_include_directories(mmruby INTERFACE
  ./mmruby/src
  ./mmruby/src/ruby-lemon-parse
  ./mmruby/src/mrubyc/src
  ./mmruby/src/mrubyc/src/hal_raspi_pico
)
set_target_properties(mmruby
  PROPERTIES
  VERSION ${PROJECT_VERSION}
)
