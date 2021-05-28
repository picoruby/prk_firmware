#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

static int8_t rotation = 0;
static uint8_t pin_a;
static uint8_t pin_b;

bool rotary_encoder_callback(repeating_timer_t *rt)
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
rotaty_encoder_entry()
{
  repeating_timer_t timer;
  if (!add_repeating_timer_ms(-3, rotary_encoder_callback, NULL, &timer)) {
    /* TODO: Handle the error. But how? */
  }
}

void
c_init_rotary_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  pin_a = GET_INT_ARG(1);
  pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
  multicore_launch_core1(rotaty_encoder_entry);
}

void
c_consume_rotation_rotary_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  rotation >>= 1;
  if (rotation > 0) {
    rotation--;
    SET_INT_RETURN(1);
  } else if (rotation < 0) {
    rotation++;
    SET_INT_RETURN(-1);
  } else {
    SET_INT_RETURN(0);
  }
}
