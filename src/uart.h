#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "uart_rx.pio.h"

/* mruby/c VM */
#include <mrubyc.h>

void c_uart_rx_init(mrb_vm *vm, mrb_value *v, int argc);

void c_uart_tx_init(mrb_vm *vm, mrb_value *v, int argc);

void c_uart_putc_raw(mrb_vm *vm, mrb_value *v, int argc);

void c_uart_getc(mrb_vm *vm, mrb_value *v, int argc);

#define UART_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "uart_rx_init",  c_uart_rx_init);  \
  mrbc_define_method(0, mrbc_class_object, "uart_tx_init",  c_uart_tx_init);  \
  mrbc_define_method(0, mrbc_class_object, "uart_putc_raw", c_uart_putc_raw); \
  mrbc_define_method(0, mrbc_class_object, "uart_getc",     c_uart_getc);     \
} while (0)
