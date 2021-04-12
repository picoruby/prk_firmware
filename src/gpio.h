#include "hardware/gpio.h"

/* mruby/c VM */
#include <mrubyc.h>

void c_gpio_get(mrb_vm *vm, mrb_value *v, int argc);

void c_gpio_init(mrb_vm *vm, mrb_value *v, int argc);

void c_gpio_set_dir(mrb_vm *vm, mrb_value *v, int argc);

void c_gpio_pull_up(mrb_vm *vm, mrb_value *v, int argc);

void c_gpio_put(mrb_vm *vm, mrb_value *v, int argc);

#define GPIO_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "gpio_init",     c_gpio_init);    \
  mrbc_define_method(0, mrbc_class_object, "gpio_set_dir",  c_gpio_set_dir); \
  mrbc_define_method(0, mrbc_class_object, "gpio_pull_up",  c_gpio_pull_up); \
  mrbc_define_method(0, mrbc_class_object, "gpio_put",      c_gpio_put);     \
  mrbc_define_method(0, mrbc_class_object, "gpio_get",      c_gpio_get);     \
} while (0)

