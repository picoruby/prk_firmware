/* mruby/c VM */
#include <mrubyc.h>

void c_mutual_uart_at_my_own_risk_eq(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_partner_init(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_anchor_init(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_partner_push8(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_partner(mrb_vm *vm, mrb_value *v, int argc);
void c_uart_anchor(mrb_vm *vm, mrb_value *v, int argc);

#define UART_INIT() do { \
  mrbc_define_method(0, mrbc_class_Keyboard, "mutual_uart_at_my_own_risk=", \
                                                                   c_mutual_uart_at_my_own_risk_eq);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_anchor_init",   c_uart_anchor_init);   \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_partner_init",  c_uart_partner_init);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_anchor",        c_uart_anchor);        \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_partner",       c_uart_partner);       \
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_partner_push8", c_uart_partner_push8); \
} while (0)
