#include <stdio.h>
#include "pico/stdlib.h"
#include "bsp/board.h"

/* mruby/c VM */
#include <mrubyc.h>

/* pico mruby compiler */
#include <picorbc.h>

/* C */
#include "usb.h"
#include "uart.h"

/* ruby */
#include "ruby/led.c"
#include "ruby/keyboard.c"
#include "ruby/keymap.c"
#include "ruby/tud.c"

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
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

#define MEMORY_SIZE (1024*130)

static uint8_t memory_pool[MEMORY_SIZE];

int main() {
  stdio_init_all();
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis",  c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "gpio_init",     c_gpio_init);
  mrbc_define_method(0, mrbc_class_object, "gpio_set_dir",  c_gpio_set_dir);
  mrbc_define_method(0, mrbc_class_object, "gpio_pull_up",  c_gpio_pull_up);
  mrbc_define_method(0, mrbc_class_object, "gpio_put",      c_gpio_put);
  mrbc_define_method(0, mrbc_class_object, "gpio_get",      c_gpio_get);
  mrbc_define_method(0, mrbc_class_object, "tud_task",      c_tud_task);
  mrbc_define_method(0, mrbc_class_object, "report_hid",    c_report_hid);
  mrbc_define_method(0, mrbc_class_object, "tud_mounted?",  c_tud_mounted_q);
  mrbc_define_method(0, mrbc_class_object, "uart_rx_init",  c_uart_rx_init);
  mrbc_define_method(0, mrbc_class_object, "uart_tx_init",  c_uart_tx_init);
  mrbc_define_method(0, mrbc_class_object, "uart_putc_raw", c_uart_putc_raw);
  mrbc_define_method(0, mrbc_class_object, "uart_getc",     c_uart_getc);
  mrbc_create_task(keyboard, 0);
  mrbc_create_task(led, 0);
  mrbc_create_task(tud, 0);
  mrbc_create_task(keymap, 0);
  mrbc_run();
  return 0;
}
