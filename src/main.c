/* PicoRuby compiler */
#include <picorbc.h>

/* C */
#include "gpio.h"
#include "usb.h"
#include "uart.h"
#include "ws2812.h"
#include "rotary_encoder.h"

/* ruby */
#include "ruby/lib/keyboard.c"
#include "ruby/lib/keymap.c"
#include "ruby/lib/tud.c"
#include "ruby/lib/rgb.c"
#include "ruby/lib/rotary_encoder.c"

void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

void
c_rand(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

mrbc_tcb *tcb_rgb; /* from ws2812.h */

int main() {
  stdio_init_all();
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "rand",         c_rand);
  GPIO_INIT();
  USB_INIT();
  UART_INIT();
  WS2812_INIT();
  ROTARY_ENCODER_INIT();
  tcb_rgb = mrbc_create_task(rgb, 0);
  mrbc_create_task(rotary_encoder, 0);
  mrbc_create_task(tud, 0);
  mrbc_create_task(keyboard, 0);
  mrbc_create_task(keymap, 0);
  mrbc_run();
  return 0;
}
