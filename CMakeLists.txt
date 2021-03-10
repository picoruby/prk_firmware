cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(prk_firmware)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_subdirectory(./lib)

add_executable(prk_firmware main.c)

add_custom_target(mrb_lib
  COMMAND mrbc -E -Bled led.rb
  COMMAND mrbc -E -Btud tud.rb
  COMMAND mrbc -E -Bhid hid.rb
  WORKING_DIRECTORY ../mrb_lib
)

add_dependencies(prk_firmware mmruby mrb_lib)

target_link_libraries(prk_firmware pico_stdlib tinyusb_device tinyusb_board mmruby)

include_directories(prk_firmware PRIVATE ${CMAKE_CURRENT_LIST_DIR} ./lib/mmruby/src/mrubyc/src)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(prk_firmware)
