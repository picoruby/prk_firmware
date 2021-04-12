/* PicoRuby compiler */
#include <picorbc.h>

/* C */
#include "gpio.h"
#include "usb.h"
#include "uart.h"
//TODO: #include "pwm.h"
//TODO: #include "spi.h"

/* ruby */
#include "ruby/keyboard.c"
#include "ruby/keymap.c"
#include "ruby/tud.c"

void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

#define MEMORY_SIZE (1024*180)

static uint8_t memory_pool[MEMORY_SIZE];

int main() {
  stdio_init_all();
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis",  c_board_millis);
  GPIO_INIT();
  USB_INIT();
  UART_INIT();
  mrbc_create_task(keyboard, 0);
  mrbc_create_task(tud, 0);
  mrbc_create_task(keymap, 0);
  mrbc_run();
  return 0;
}
