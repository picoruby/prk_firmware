/* mruby/c VM */
#include <mrubyc.h>

void c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_off(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_show(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_fill(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_rand_fill(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_set_pixel_at(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_rotate(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_save(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_restore(mrb_vm *vm, mrb_value *v, int argc);

#define WS2812_INIT() do { \
  mrbc_class *mrbc_class_RGB = mrbc_define_class(0, "RGB", mrbc_class_object);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_init",         c_ws2812_init);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_off",          c_ws2812_off);          \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_show",         c_ws2812_show);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_fill",         c_ws2812_fill);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_rand_fill",    c_ws2812_rand_fill);    \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_set_pixel_at", c_ws2812_set_pixel_at); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_rotate",       c_ws2812_rotate);       \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_save",         c_ws2812_save);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_restore",      c_ws2812_restore);      \
} while (0)

