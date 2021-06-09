#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

static int8_t rotation = 0;
static uint8_t pin_a;
static uint8_t pin_b;

void
c_read_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  static int8_t status_table[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
  static int8_t prev_status = 0;
  int8_t current_status;
  prev_status <<= 2;
  current_status = gpio_get(pin_b) * 2 + gpio_get(pin_a);
  prev_status |= (current_status & 0b0011);
  rotation += status_table[(prev_status & 0b1111)];
}

void
c_init_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  pin_a = GET_INT_ARG(1);
  pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
}

void
c_encoder_value(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rotation >> 1);
}

void
c_reset_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  rotation = 0;
}
