#!/bin/sh

touch lib/picoruby/src/mrubyc/src/mrblib.c
touch lib/picoruby/src/mrubyc/src/hal_user_reserved/hal.c
cd build
cmake ..
cd -
rm lib/picoruby/src/mrubyc/src/mrblib.c
rm lib/picoruby/src/mrubyc/src/hal_user_reserved/hal.c
