cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(prk_firmware)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(prk_firmware
  src/main.c
  src/gpio.c
  src/usb.c
  src/uart.c
  src/rgb.c
)

pico_generate_pio_header(prk_firmware
  ${CMAKE_CURRENT_LIST_DIR}/src/uart_tx.pio
)

set(RBC ${CMAKE_CURRENT_SOURCE_DIR}/lib/picoruby/build/host-production/bin/picorbc)
#set(RBC RBENV_VERSION=mruby-2.1.1 mrbc -E)

add_custom_target(ruby
  COMMAND ${RBC} -Bkeyboard keyboard.rb
  COMMAND ${RBC} -Btud tud.rb
  COMMAND ${RBC} -Brgb rgb.rb
  COMMAND ${RBC} -Bkeymap ${PROJECT_BINARY_DIR}/../keymap.rb
  COMMAND mv ${PROJECT_BINARY_DIR}/../keymap.c ./
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby
)

add_dependencies(prk_firmware
  picoruby
  ruby
)

target_link_libraries(prk_firmware
  pico_stdlib
  tinyusb_device
  tinyusb_board
  picoruby
  hardware_pio
  hardware_pwm
)

include_directories(prk_firmware
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/picoruby/src/mrubyc/src
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(prk_firmware)
