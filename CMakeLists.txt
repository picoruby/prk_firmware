cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

add_definitions(
  -DNDEBUG
  -DMRBC_USE_HAL_RP2040
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMAX_REGS_SIZE=256
  -DMAX_VM_COUNT=255
  -DMAX_SYMBOLS_COUNT=1000
  -DMRBC_CONVERT_CRLF
  -DMRBC_USE_MATH
  -DPICORBC_PTR_SIZE=4
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
execute_process (COMMAND git describe --tags --abbrev=0 OUTPUT_VARIABLE CMAKE_VERSION_NUMBER OUTPUT_STRIP_TRAILING_WHITESPACE)
set (PRK_VERSION   ${CMAKE_VERSION_NUMBER})
set (PRK_BUILDDATE ${CMAKE_BUILDDATE})
set (PRK_REVISION  ${CMAKE_REVISION})
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h")

if(PRK_NO_MSC)
  project("prk_firmware-${PRK_VERSION}-${PRK_BUILDDATE}-no_msc")
else()
  project("prk_firmware-${PRK_VERSION}-${PRK_BUILDDATE}-${PRK_REVISION}")
endif()

file(GLOB SRCS src/*.c)
add_executable(${PROJECT_NAME} ${SRCS})

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/uart_tx.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/sounder.pio)

set(RBC ${CMAKE_CURRENT_SOURCE_DIR}/lib/picoruby/bin/picorbc)
#set(RBC RBENV_VERSION=mruby-3.1.0 mrbc) # In case of PicoRuby compiler enbugged 😵

if(PRK_NO_MSC)
  add_custom_target(keymap
    COMMAND ${RBC} -Bkeymap         ${PROJECT_BINARY_DIR}/../keymap.rb
    COMMAND mv ${PROJECT_BINARY_DIR}/../keymap.c ./
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/
  )
else()
  add_custom_target(keymap)
endif()

add_custom_target(ext
  COMMAND ${RBC} -Bobject         object.rb
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/ext
)
add_custom_target(models
  COMMAND ${RBC} -Bfloat_ext      float_ext.rb
  COMMAND ${RBC} -Bkeyboard       keyboard.rb
  COMMAND ${RBC} -Bvia            via.rb
  COMMAND ${RBC} -Bconsumer_key   consumer_key.rb
  COMMAND ${RBC} -Brgb            rgb.rb
  COMMAND ${RBC} -Brotary_encoder rotary_encoder.rb
  COMMAND ${RBC} -Bbuffer         buffer.rb
  COMMAND ${RBC} -Bdebounce       debounce.rb
  COMMAND ${RBC} -Bjoystick       joystick.rb
  COMMAND ${RBC} -Bsounder        sounder.rb
  COMMAND ${RBC} -Bmml            mml.rb
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/models
)

add_custom_target(tasks
  COMMAND ${RBC} -Busb_task       usb_task.rb
  COMMAND ${RBC} -Brgb_task       rgb_task.rb
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/ruby/app/tasks
)

add_custom_target(ruby
)

add_dependencies(ruby
  ext
  models
  tasks
)
add_dependencies(${PROJECT_NAME}
  ruby
)
if(PRK_NO_MSC)
  add_dependencies(${PROJECT_NAME}
    keymap
  )
endif()

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/r2p2-cortex-m0plus/lib
)

target_link_libraries(${PROJECT_NAME}
  pico_stdlib
  pico_bootsel_via_double_reset
  tinyusb_device
  tinyusb_board
  hardware_pio
  hardware_pwm
  pico_multicore
  hardware_flash
  hardware_adc
  hardware_dma
  hardware_sync
  hardware_irq
  mruby
)

include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/lib/picoruby/build/repos/host/mruby-pico-compiler/include
  ${CMAKE_CURRENT_LIST_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/src
  ${CMAKE_CURRENT_LIST_DIR}/lib/picoruby/mrbgems/picoruby-sandbox/src
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT_NAME}.uf2
  COMMAND zip -r ${PROJECT_NAME}.uf2.zip ${PROJECT_NAME}.uf2
)
