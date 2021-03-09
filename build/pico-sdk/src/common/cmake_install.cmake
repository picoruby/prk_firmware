# Install script for directory: /home/hasumi/pico/pico-sdk/src/common

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/boot_picoboot/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/boot_uf2/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_base/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_bit_ops/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_binary_info/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_divider/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_sync/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_time/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_util/cmake_install.cmake")
  include("/home/hasumi/work/pico-work/hid/build/pico-sdk/src/common/pico_stdlib/cmake_install.cmake")

endif()

