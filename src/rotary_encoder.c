#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

void
c_read_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  static int8_t status_table[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
  static int8_t prev_status = 0;
  int8_t current_status;
  prev_status <<= 2;
  current_status = gpio_get(GET_INT_ARG(2)) * 2 + gpio_get(GET_INT_ARG(1));
  prev_status |= (current_status & 0b0011);
  SET_INT_RETURN(status_table[(prev_status & 0b1111)]);
}

void
c_init_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8_t pin_a = GET_INT_ARG(1);
  uint8_t pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
}
