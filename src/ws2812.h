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
  mrbc_define_method(0, mrbc_class_object, "ws2812_show",      c_ws2812_show);      \
  mrbc_define_method(0, mrbc_class_object, "ws2812_fill",       c_ws2812_fill); \
  mrbc_define_method(0, mrbc_class_object, "ws2812_rand_fill",  c_ws2812_rand_fill); \
  mrbc_define_method(0, mrbc_class_object, "ws2812_set_pixel_at", c_ws2812_set_pixel_at); \
  mrbc_define_method(0, mrbc_class_object, "ws2812_rotate",       c_ws2812_rotate); \
  mrbc_define_method(0, mrbc_class_object, "ws2812_save",       c_ws2812_save); \
  mrbc_define_method(0, mrbc_class_object, "ws2812_restore",       c_ws2812_restore); \
} while (0)

extern mrbc_tcb *tcb_rgb;

void c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_off(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_resume(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_show(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_fill(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_rand_fill(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_set_pixel_at(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_rotate(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_save(mrb_vm *vm, mrb_value *v, int argc);

void c_ws2812_restore(mrb_vm *vm, mrb_value *v, int argc);
