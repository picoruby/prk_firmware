#include <mrubyc.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define PWM_BASE_HZ (1000000UL)

static uint pwm_slice_num;
static uint32_t pwm_remain_count = 0;
static uint32_t pwm_wrap_count = 1;

static void
on_pwm_wrap(void)
{
  pwm_clear_irq(pwm_slice_num);
  if (++pwm_wrap_count>pwm_remain_count)
  {
    pwm_set_enabled(pwm_slice_num, false);
  }
}

void
c_pwm_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8_t pin = GET_INT_ARG(1);
  uint16_t tone_hz = GET_INT_ARG(2);
  uint16_t pwm_remain_ms = GET_INT_ARG(3);

  gpio_set_function(pin, GPIO_FUNC_PWM);
  pwm_slice_num = pwm_gpio_to_slice_num(pin);
  
  if (pwm_remain_ms>0) {
    pwm_remain_count = pwm_remain_ms*tone_hz/1000;
    pwm_clear_irq(pwm_slice_num);
    pwm_set_irq_enabled(pwm_slice_num, true);
    //irq_add_shared_handler(PWM_IRQ_WRAP, on_pwm_wrap, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    irq_set_enabled(PWM_IRQ_WRAP, true);
  } else {
    pwm_set_irq_enabled(pwm_slice_num, false);
  }
  
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, (float)clock_get_hz(clk_sys)/PWM_BASE_HZ);
  pwm_config_set_wrap(&config, PWM_BASE_HZ/tone_hz);
  
  pwm_init(pwm_slice_num, &config, false);

  pwm_set_gpio_level(pin, (PWM_BASE_HZ/tone_hz)>>1);
}

void
c_pwm_enable(mrb_vm *vm, mrb_value *v, int argc)
{
  pwm_wrap_count = 1;
  pwm_set_enabled(pwm_slice_num, true);
}
