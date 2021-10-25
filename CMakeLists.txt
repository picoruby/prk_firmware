cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(prk_firmware)

add_definitions(
  -DNDEBUG
  -DMRBC_USE_HAL_USER_RESERVED
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMAX_REGS_SIZE=256
  -DMAX_SYMBOLS_COUNT=550
  -DMAX_VM_COUNT=10
  -DMRBC_CONVERT_CRLF
)
if(PRK_NO_MSC)
  add_definitions(-DPRK_NO_MSC)
endif()

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

#####################################################
# project specific configuration from here

execute_process (COMMAND date +%Y%m%d OUTPUT_VARIABLE CMAKE_BUILDDATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE CMAKE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)
set (PRK_VERSION   0.9.5)
set (PRK_BUILDDATE ${CMAKE_BUILDDATE})
set (PRK_REVISION  ${CMAKE_REVISION})
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h")

if(PRK_NO_MSC)
  set(PROJECT "prk_firmware-${PRK_VERSION}-${PRK_BUILDDATE}-no_msc")
else()
  set(PROJECT "prk_firmware-${PRK_VERSION}-${PRK_BUILDDATE}-${PRK_REVISION}")
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(${PROJECT}
  src/main.c
  src/msc_disk.c
  src/usb_descriptors.c
  src/gpio.c
  src/uart.c
  src/ws2812.c
  src/rotary_encoder.c
  lib/picoruby/cli/sandbox.c
)

pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/src/uart_tx.pio)
pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)

set(RBC ${CMAKE_CURRENT_SOURCE_DIR}/lib/picoruby/build/bin/host-production/alloc_libc/picorbc)
#set(RBC RBENV_VERSION=mruby-3.0.0 mrbc) # In case of PicoRuby compiler enbugged 😵

add_custom_target(steep
  COMMAND bundle exec steep -h || bundle install
  COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} rake steep
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby
)

add_custom_target(mrubyc-test
  COMMAND bundle exec mrubyc-test version || bundle install
  COMMAND ${CMAKE_COMMAND} -E env PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR} rake test
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby
)

if(PRK_NO_MSC)
  add_custom_target(keymap
    COMMAND ${RBC} -Bkeymap         ${PROJECT_BINARY_DIR}/../keymap.rb
    COMMAND mv ${PROJECT_BINARY_DIR}/../keymap.c ./
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/
  )
else()
  add_custom_target(keymap)
endif()

add_custom_target(models
  COMMAND ${RBC} -Bcore           core.rb
  COMMAND ${RBC} -Bkeyboard       keyboard.rb
  COMMAND ${RBC} -Brgb            rgb.rb
  COMMAND ${RBC} -Brotary_encoder rotary_encoder.rb
  COMMAND ${RBC} -Bbuffer         buffer.rb
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/models
)

add_custom_target(tasks
  COMMAND ${RBC} -Busb_task       usb_task.rb
  COMMAND ${RBC} -Brgb_task       rgb_task.rb
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/tasks
)

add_custom_target(ruby
)

#add_custom_target(sandbox
#  COMMAND ${RBC} -Bsandbox        sandbox.rb
#  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/picoruby/cli/ruby
#)

add_dependencies(ruby
  models
  tasks
  steep
  mrubyc-test
)
add_dependencies(${PROJECT}
  picoruby
  ruby
  #  sandbox
)
if(PRK_NO_MSC)
  add_dependencies(${PROJECT}
    keymap
  )
endif()

target_link_libraries(${PROJECT}
  pico_stdlib
  pico_bootsel_via_double_reset
  tinyusb_device
  tinyusb_board
  picoruby
  hardware_pio
  hardware_pwm
  pico_multicore
  hardware_flash
)

include_directories(${PROJECT}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/lib/picoruby/src/mrubyc/src
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT})

add_custom_command(
  TARGET ${PROJECT}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT}.uf2
  COMMAND zip -r ${PROJECT}.uf2.zip ${PROJECT}.uf2
)
