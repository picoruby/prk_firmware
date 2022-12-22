#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "../include/rotary_encoder.h"
#include "picoruby-prk-rotary_encoder/include/prk-rotary_encoder.h"

/*
 * TODO: Reconsuruct these functions with picoruby-gpio gem
 */

void
RotaryEncoder_gpio_init(uint32_t pin)
{
  gpio_init((uint)pin);
}

bool
RotaryEncoder_gpio_get(uint32_t pin)
{
  return gpio_get((uint)pin);
}

void
RotaryEncoder_gpio_set_dir_in(uint32_t pin)
{
  gpio_set_dir((uint)pin, GPIO_IN);
}

void
RotaryEncoder_gpio_pull_up(uint32_t pin)
{
  gpio_pull_up((uint)pin);
}

void
RotaryEncoder_gpio_set_irq_enabled_with_callback(uint32_t pin, /* uint32_t event_mask, */ bool enabled, void *callback)
{
  uint32_t events = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;
  gpio_set_irq_enabled_with_callback((uint)pin, events, enabled, (gpio_irq_callback_t)callback);
}

