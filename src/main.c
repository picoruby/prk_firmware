/* PicoRuby compiler */
#include <picorbc.h>

/* Raspi SDK */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "hardware/clocks.h"

/* mrbc_class */
#include "msc_disk.h"
#include "gpio.h"
#include "usb.h"
#include "uart.h"
#include "ws2812.h"
#include "rotary_encoder.h"
#include "../lib/picoruby/cli/sandbox.h"

/* ruby */
/* models */
#include "ruby/lib/core.c"
#include "ruby/lib/keyboard.c"
#include "ruby/lib/rotary_encoder.c"
#include "ruby/lib/rgb.c"
#include "../lib/picoruby/cli/ruby/buffer.c"
/* tasks */
#include "ruby/lib/tud.c"
#include "ruby/lib/rgb_task.c"
#include "ruby/lib/keymap.c"

void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

void
c_srand(mrb_vm *vm, mrb_value *v, int argc)
{
  srand(GET_INT_ARG(1));
}

void
c_rand(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

void
mrbc_load_model(const uint8_t *mrb)
{
  mrbc_vm *vm = mrbc_vm_open(NULL);
  if( vm == 0 ) {
    console_printf("Error: Can't open VM.\n");
    return;
  }
  if( mrbc_load_mrb(vm, mrb) != 0 ) {
    console_printf("Error: Illegal bytecode.\n");
    return;
  }
  mrbc_vm_begin(vm);
  mrbc_vm_run(vm);
  mrbc_raw_free(vm);
}

mrbc_tcb *tcb_rgb; /* from ws2812.h */

int main() {
  stdio_init_all();
  board_init();
  tusb_init();
  msc_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "rand",         c_rand);
  MSC_INIT();
  GPIO_INIT();
  USB_INIT();
  UART_INIT();
  WS2812_INIT();
  ROTARY_ENCODER_INIT();
  SANDBOX_INIT();
  mrbc_load_model(core);
  mrbc_load_model(rgb);
  mrbc_load_model(buffer);
  mrbc_load_model(rotary_encoder);
  mrbc_load_model(keyboard);
  mrbc_create_task(tud, 0);
  tcb_rgb = mrbc_create_task(rgb_task, 0);
  create_sandbox();
  mrbc_create_task(keymap, 0);
  mrbc_run();
  return 0;
}
