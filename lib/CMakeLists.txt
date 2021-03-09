cmake_minimum_required(VERSION 3.13)
include(../pico_sdk_import.cmake)
project(mrubyc
  VERSION 0.0.1
  DESCRIPTION "mruby/c VM implementation"
  LANGUAGES C
)
add_definitions(-DMRBC_USE_HAL_RASPI_PICO -DMRBC_REQUIRE_32BIT_ALIGNMENT)
add_library(mrubyc STATIC
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
)
target_link_libraries(mrubyc pico_stdlib)
target_compile_features(mrubyc PRIVATE)
target_include_directories(mrubyc INTERFACE ./mmruby/src/mrubyc/src ./mmruby/src/mrubyc/src/hal_raspi_pico)
set_target_properties(mrubyc
  PROPERTIES
  VERSION ${PROJECT_VERSION}
)
