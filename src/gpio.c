#include "hardware/gpio.h"
#include <mrubyc.h>

//      GPIO_IN          0b000 (defined in the SDK)
#define GPIO_IN_PULLUP   0b010
#define GPIO_IN_PULLDOWN 0b110
//      GPIO_OUT         0b001 (defined in the SDK)
#define GPIO_OUT_LO      0b011
#define GPIO_OUT_HI      0b101

static void
c_gpio_get(mrb_vm *vm, mrb_value *v, int argc)
{
  int gpio = GET_INT_ARG(1);
  if (gpio_get(gpio)) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

static void
c_gpio_init(mrb_vm *vm, mrb_value *v, int argc)
{
  gpio_init(GET_INT_ARG(1));
}

static void
c_gpio_set_dir(mrb_vm *vm, mrb_value *v, int argc)
{
  if (GET_INT_ARG(2)&1 == GPIO_OUT) {
    gpio_set_dir(GET_INT_ARG(1), true);
    if (GET_INT_ARG(2) == GPIO_OUT_LO) {
      gpio_put(GET_INT_ARG(1), 0);
    } else if (GET_INT_ARG(2) == GPIO_OUT_HI) {
      gpio_put(GET_INT_ARG(1), 1);
    }
  } else {
    gpio_set_dir(GET_INT_ARG(1), false);
    if (GET_INT_ARG(2) == GPIO_IN_PULLUP) {
      gpio_pull_up(GET_INT_ARG(1));
    } else if (GET_INT_ARG(2) == GPIO_IN_PULLDOWN) {
      gpio_pull_down(GET_INT_ARG(1));
    }
  }
}

static void
c_gpio_pull_up(mrb_vm *vm, mrb_value *v, int argc)
{
  gpio_pull_up(GET_INT_ARG(1));
}

static void
c_gpio_put(mrb_vm *vm, mrb_value *v, int argc)
{
  int gpio = GET_INT_ARG(1);
  int value = GET_INT_ARG(2);
  gpio_put(gpio, value);
}

void
prk_init_gpio(void)
{
  mrbc_define_method(0, mrbc_class_object, "gpio_init",    c_gpio_init);
  mrbc_define_method(0, mrbc_class_object, "gpio_set_dir", c_gpio_set_dir);
  mrbc_define_method(0, mrbc_class_object, "gpio_pull_up", c_gpio_pull_up);
  mrbc_define_method(0, mrbc_class_object, "gpio_put",     c_gpio_put);
  mrbc_define_method(0, mrbc_class_object, "gpio_get",     c_gpio_get);
}

