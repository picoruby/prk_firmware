#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

/* mruby/c VM */
#include <mrubyc.h>

#define WS2812_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "ws2812_init",      c_ws2812_init);      \
  mrbc_define_method(0, mrbc_class_object, "ws2812_off",       c_ws2812_off);       \
  mrbc_define_method(0, mrbc_class_object, "ws2812_resume",    c_ws2812_resume);    \
  mrbc_define_method(0, mrbc_class_object, "ws2812_snakes",    c_ws2812_snakes);    \
  mrbc_define_method(0, mrbc_class_object, "ws2812_put_pixel", c_ws2812_put_pixel); \
} while (0)

extern mrbc_tcb *tcb_rgb;

void c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_off(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_snakes(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_resume(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_put_pixel(mrb_vm *vm, mrb_value *v, int argc);

