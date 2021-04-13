/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "rgb.h"

static uint8_t rgb_pin = 25;

void on_pwm_wrap() {
    static int fade = 0;
    static bool going_up = true;
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(rgb_pin));

    if (going_up) {
        ++fade;
        if (fade > 255) {
            fade = 255;
            going_up = false;
        }
    } else {
        --fade;
        if (fade < 0) {
            fade = 0;
            going_up = true;
        }
    }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(rgb_pin, fade * fade);
}

void
c_rgb_init(mrb_vm *vm, mrb_value *v, int argc)
{
  rgb_pin = GET_INT_ARG(1);

  // Tell the LED pin that the PWM is in charge of its value.
  gpio_set_function(rgb_pin, GPIO_FUNC_PWM);
  // Figure out which slice we just connected to the LED pin
  uint slice_num = pwm_gpio_to_slice_num(rgb_pin);

  // Mask our slice's IRQ output into the PWM block's single interrupt line,
  // and register our interrupt handler
  pwm_clear_irq(slice_num);
  pwm_set_irq_enabled(slice_num, true);
  irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
  irq_set_enabled(PWM_IRQ_WRAP, true);

  // Get some sensible defaults for the slice configuration. By default, the
  // counter is allowed to wrap over its maximum range (0 to 2**16-1)
  pwm_config config = pwm_get_default_config();
  // Set divider, reduces counter clock to sysclock/this value
  pwm_config_set_clkdiv(&config, 4.f);
  // Load the configuration into our PWM slice, and set it running.
  pwm_init(slice_num, &config, true);
}

void
c_tight_loop_contents(mrb_vm *vm, mrb_value *v, int argc)
{
  tight_loop_contents();
}


void
c_resume_rgb(mrb_vm *vm, mrb_value *v, int argc)
{
  if (tcb_rgb == NULL) return;
  mrbc_resume_task(tcb_rgb);
}
