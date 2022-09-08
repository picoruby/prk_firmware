/* mruby/c VM */
#include <mrubyc.h>

void c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_show(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_fill(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_rand_show(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_set_pixel_at(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_rotate_swirl(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_reset_swirl_index(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_circle(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_add_matrix_pixel_at(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_init_pixel_distance(mrb_vm *vm, mrb_value *v, int argc);
void c_ws2812_circle_set_center(mrb_vm *vm, mrb_value *v, int argc);

#define WS2812_INIT() do { \
  mrbc_class *mrbc_class_RGB = mrbc_define_class(0, "RGB", mrbc_class_object);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_init",         c_ws2812_init);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_show",         c_ws2812_show);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_fill",         c_ws2812_fill);         \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_rand_show",    c_ws2812_rand_show);    \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_set_pixel_at", c_ws2812_set_pixel_at); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_rotate_swirl", c_ws2812_rotate_swirl); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_reset_swirl_index", c_ws2812_reset_swirl_index); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_add_matrix_pixel_at", c_ws2812_add_matrix_pixel_at); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_init_pixel_distance", c_ws2812_init_pixel_distance); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_circle",       c_ws2812_circle); \
  mrbc_define_method(0, mrbc_class_RGB, "ws2812_circle_set_center", c_ws2812_circle_set_center); \
} while (0)

