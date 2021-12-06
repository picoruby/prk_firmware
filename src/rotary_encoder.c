#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

static int8_t encoder_index = 0;
static int8_t status_table[] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
static int8_t prev_status[] = {0,0,0,0,0};

void
c_read_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  int8_t current_status;
  encoder_index = GET_INT_ARG(1);
  prev_status[encoder_index] <<= 2;
  current_status = gpio_get(GET_INT_ARG(3)) * 2 + gpio_get(GET_INT_ARG(2));
  prev_status[encoder_index] |= (current_status & 0b0011);
  SET_INT_RETURN(status_table[(prev_status[encoder_index] & 0b1111)]);
}

void
c_init_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  if (encoder_index > 4) {
    console_printf("RotaryEncoderLimitationError. The maximum count of RotatyEncoder is 5.\n");
  }
  uint8_t pin_a = GET_INT_ARG(1);
  uint8_t pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
  encoder_index++;
}
