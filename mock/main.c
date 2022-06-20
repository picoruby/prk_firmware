#include <mrubyc.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/ruby/lib/keyboard.c"
#include "../src/ruby/lib/keymap.c"
#include "../src/ruby/lib/tud.c"
#include "../src/ruby/lib/rgb.c"
#include "../src/ruby/lib/rotary_encoder.c"

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / 1000)
void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(clock() / CLOCKS_PER_MSEC);
}

void
c_rand(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

void
c_gpio_get(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(1);
}

void
c_gpio_init(mrb_vm *vm, mrb_value *v, int argc)
{
}

void
c_gpio_set_dir(mrb_vm *vm, mrb_value *v, int argc)
{
}

void
c_gpio_pull_up(mrb_vm *vm, mrb_value *v, int argc)
{
}

void
c_gpio_put(mrb_vm *vm, mrb_value *v, int argc)
{
}

#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

mrbc_tcb *tcb_rgb; /* from ws2812.h */

int main() {
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "rand",         c_rand);
  mrbc_define_method(0, mrbc_class_object, "gpio_init",    c_gpio_init);    \
  mrbc_define_method(0, mrbc_class_object, "gpio_set_dir", c_gpio_set_dir); \
  mrbc_define_method(0, mrbc_class_object, "gpio_pull_up", c_gpio_pull_up); \
  mrbc_define_method(0, mrbc_class_object, "gpio_put",     c_gpio_put);     \
  mrbc_define_method(0, mrbc_class_object, "gpio_get",     c_gpio_get);     \
//  GPIO_INIT();
//  USB_INIT();
//  UART_INIT();
//  WS2812_INIT();
//  ROTARY_ENCODER_INIT();
//  tcb_rgb = mrbc_create_task(rgb, 0);
//  mrbc_create_task(rotary_encoder, 0);
//  mrbc_create_task(tud, 0);
  mrbc_create_task(keyboard, 0);
  mrbc_create_task(keymap, 0);
  mrbc_run();
  return 0;
}
