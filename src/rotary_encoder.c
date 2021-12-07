#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

/*
 * Reference:
 *   https://coskxlabsite.stars.ne.jp/html/for_students/H8/rotaryencoderTips.html
 */
const static int8_t status_table[] = {0, 1,-1, 0,-1, 0, 0, 1, 1, 0, 0,-1, 0,-1, 1, 0};
const static int8_t error_table[]  = {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 ,0, 0};
static int8_t encoder_count = 0;
static int8_t prev_status[] = {0,0,0,0,0};

void
RotaryEncoder_reset(void)
{
  encoder_count = 0;
  for (int i = 0; i < 5; i++) prev_status[i] = 0;
}

void
c_read_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8_t encoder_index = GET_INT_ARG(1);
  prev_status[encoder_index] <<= 2;
  prev_status[encoder_index] |= ( (gpio_get(GET_INT_ARG(3))*2 + gpio_get(GET_INT_ARG(2))) & 0b0011);
  prev_status[encoder_index] &= 0b1111;
  if (error_table[prev_status[encoder_index]] == 1) {
    SET_INT_RETURN(0);
  } else {
    SET_INT_RETURN(status_table[prev_status[encoder_index]]);
  }
}

void
c_init_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  if (encoder_count > 4) {
    console_printf("RotaryEncoderLimitationError.\n");
    return;
  }
  uint8_t pin_a = GET_INT_ARG(1);
  uint8_t pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
  encoder_count++;
}
