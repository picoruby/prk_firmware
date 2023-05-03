cmake_minimum_required(VERSION 3.24)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

#####################################################
# project specific configuration from here

add_definitions(
  -DMRBC_USE_HAL_RP2040
  -DMRBC_REQUIRE_32BIT_ALIGNMENT
  -DMAX_REGS_SIZE=256
  -DMAX_VM_COUNT=255
  -DMAX_SYMBOLS_COUNT=2000
  -DMRBC_CONVERT_CRLF
  -DMRBC_USE_MATH
  -DPICORBC_PTR_SIZE=4
  -DNO_CLOCK_GETTIME=1
)

execute_process (COMMAND date +%Y%m%d OUTPUT_VARIABLE CMAKE_BUILDDATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE CMAKE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git describe --tags --abbrev=0 OUTPUT_VARIABLE CMAKE_VERSION_NUMBER OUTPUT_STRIP_TRAILING_WHITESPACE)
set (PRK_VERSION   ${CMAKE_VERSION_NUMBER})
set (PRK_BUILDDATE ${CMAKE_BUILDDATE})
set (PRK_REVISION  ${CMAKE_REVISION})
configure_file ("${CMAKE_SOURCE_DIR}/include/version.h.in" "${CMAKE_SOURCE_DIR}/include/version.h")

if(DEFINED ENV{PICORUBY_NO_MSC})
  add_definitions(-DPICORUBY_NO_MSC)
  project("prk_firmware-no_msc-${PRK_VERSION}-${PRK_BUILDDATE}-${PRK_REVISION}")
else()
  add_definitions(-DPICORUBY_MSC_FLASH)
  if(DEFINED ENV{PICORUBY_SQLITE3})
    add_definitions(-DPICORUBY_SQLITE3)
    project("prk_firmware-sqlite3-${PRK_VERSION}-${PRK_BUILDDATE}-${PRK_REVISION}")
  else()
    project("prk_firmware-${PRK_VERSION}-${PRK_BUILDDATE}-${PRK_REVISION}")
  endif()
endif()

# Initializing the Raspberry Pi Pico SDK should happen after project created
pico_sdk_init()

file(GLOB SRCS src/*.c)
add_executable(${PROJECT_NAME}
  ${SRCS}
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/flash_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/sd_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/ports/rp2040/msc_disk.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-gpio/ports/rp2040/gpio.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-i2c/ports/rp2040/i2c.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-spi/ports/rp2040/spi.c
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-adc/ports/rp2040/adc.c
)

set(PICORBC ${CMAKE_SOURCE_DIR}/lib/picoruby/bin/picorbc)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ws2812.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/sounder.pio)

set(RBC ${CMAKE_CURRENT_SOURCE_DIR}/lib/picoruby/bin/picorbc)
#set(RBC RBENV_VERSION=mruby-3.2.0 mrbc) # In case of PicoRuby compiler enbugged 😵

# Ruby

if(DEFINED ENV{PICORUBY_NO_MSC})
  add_dependencies(${PROJECT_NAME}
    keymap
  )
  add_custom_target(keymap
    COMMAND ${RBC} -Bkeymap -o ./keymap.c ../keymap.rb
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
  target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_BINARY_DIR}
  )
endif()

add_custom_target(mrbdir ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/mrb
)

set(RUBY_TASK_FILES
  usb_task
)
foreach(rb ${RUBY_TASK_FILES})
  add_custom_target(${rb}
    COMMAND ${PICORBC} -B${rb}
      -o${CMAKE_SOURCE_DIR}/build/mrb/${rb}.c
      ${CMAKE_SOURCE_DIR}/mrblib/${rb}.rb
  )
endforeach(rb)

add_custom_target(object-ext
  COMMAND ${PICORBC} -Bobject_ext
    -o${CMAKE_SOURCE_DIR}/build/mrb/object-ext.c
    ${CMAKE_SOURCE_DIR}/mrblib/object-ext.rb
)

add_dependencies(${PROJECT_NAME}
  object-ext
  mrbdir
  ${RUBY_TASK_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/tinyusb
  ${CMAKE_SOURCE_DIR}/lib/picoruby/include/hal_no_impl
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-mrubyc/repos/mrubyc/src
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems/picoruby-filesystem-fat/lib/ff14b/source
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/$ENV{MRUBY_CONFIG}/mrbgems
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/repos/$ENV{MRUBY_CONFIG}/mruby-pico-compiler/include
  ${CMAKE_SOURCE_DIR}/lib/picoruby/mrbgems
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/lib/picoruby/build/$ENV{MRUBY_CONFIG}/lib
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
  hardware_i2c
  hardware_spi
  mruby
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND gzip -kf ${PROJECT_NAME}.uf2
  COMMAND zip -r ${PROJECT_NAME}.uf2.zip ${PROJECT_NAME}.uf2
)
