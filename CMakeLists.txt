cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(hid)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(hid
    hid.c
)

target_link_libraries(hid pico_stdlib tinyusb_device tinyusb_board)

include_directories(hid PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(hid)
