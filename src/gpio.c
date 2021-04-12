#include "gpio.h"

void
c_gpio_get(mrb_vm *vm, mrb_value *v, int argc)
{
  int gpio = GET_INT_ARG(1);
  if (gpio_get(gpio)) {
    SET_INT_RETURN(1);
  } else {
    SET_INT_RETURN(0);
  }
}

void
c_gpio_init(mrb_vm *vm, mrb_value *v, int argc)
{
  gpio_init(GET_INT_ARG(1));
}

void
c_gpio_set_dir(mrb_vm *vm, mrb_value *v, int argc)
{
  gpio_set_dir(GET_INT_ARG(1), GET_INT_ARG(2));
}

void
c_gpio_pull_up(mrb_vm *vm, mrb_value *v, int argc)
{
  gpio_pull_up(GET_INT_ARG(1));
}

void
c_gpio_put(mrb_vm *vm, mrb_value *v, int argc)
{
  int gpio = GET_INT_ARG(1);
  int value = GET_INT_ARG(2);
  gpio_put(gpio, value);
}

