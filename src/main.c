#include <stdio.h>
#include "pico/stdlib.h"
#include "bsp/board.h"

/* mruby/c VM */
#include <mrubyc.h>

/* pico mruby compiler */
#include <picorbc.h>

/* C */
#include "usb.h"

/* ruby */
#include "ruby/led.c"
#include "ruby/tud.c"
#include "ruby/hid.c"

#define COL0 6
#define COL1 7
#define ROW0 2
#define ROW1 3
#define LED  25

void
init_pins()
{
  /* LED on board*/
  gpio_init(LED);
  gpio_set_dir(LED, GPIO_OUT);

  gpio_init(ROW0);
  gpio_set_dir(ROW0, GPIO_IN);
  gpio_pull_up(ROW0);
  gpio_init(ROW1);
  gpio_set_dir(ROW1, GPIO_IN);
  gpio_pull_up(ROW1);

  gpio_init(COL0);
  gpio_set_dir(COL0, GPIO_OUT);
  gpio_put(COL0, 1);
  gpio_init(COL1);
  gpio_set_dir(COL1, GPIO_OUT);
  gpio_put(COL1, 1);

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

#define MEMORY_SIZE (1024*100)

static uint8_t memory_pool[MEMORY_SIZE];

int main() {
  stdio_init_all();
  board_init();
  tusb_init();
  init_pins();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "gpio_put",     c_gpio_put);
  mrbc_define_method(0, mrbc_class_object, "gpio_get",     c_gpio_get);
  mrbc_define_method(0, mrbc_class_object, "tud_task",     c_tud_task);
  mrbc_define_method(0, mrbc_class_object, "report_hid",   c_report_hid);
  mrbc_create_task(led, 0);
  mrbc_create_task(tud, 0);
  mrbc_create_task(hid, 0);
  mrbc_run();
  return 0;
}
