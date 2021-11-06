/* mruby/c VM */
#include <mrubyc.h>

void c_uart_rx_init(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_tx_init(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_putc_raw(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_getc(mrb_vm *vm, mrb_value *v, int argc);

void c_bi_uart_partner_init(mrb_vm *vm, mrb_value *v, int argc);
void c_bi_uart_anchor_init(mrb_vm *vm, mrb_value *v, int argc);
void c_bi_uart_partner_push(mrb_vm *vm, mrb_value *v, int argc);
void c_bi_uart_partner(mrb_vm *vm, mrb_value *v, int argc);
void c_bi_uart_anchor(mrb_vm *vm, mrb_value *v, int argc);

#define UART_INIT() do { \
  mrbc_class *mrbc_class_Keyboard = mrbc_define_class(0, "Keyboard", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_rx_init",  c_uart_rx_init);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_tx_init",  c_uart_tx_init);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_putc_raw", c_uart_putc_raw); \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_getc",     c_uart_getc);     \
  \
  mrbc_define_method(0, mrbc_class_Keyboard, "bi_uart_anchor_init",  c_bi_uart_anchor_init);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "bi_uart_partner_init", c_bi_uart_partner_init); \
  mrbc_define_method(0, mrbc_class_Keyboard, "bi_uart_anchor",       c_bi_uart_anchor);       \
  mrbc_define_method(0, mrbc_class_Keyboard, "bi_uart_partner",      c_bi_uart_partner);      \
  mrbc_define_method(0, mrbc_class_Keyboard, "bi_uart_partner_push", c_bi_uart_partner_push); \
} while (0)
