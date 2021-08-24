#include <mrubyc.h>
#include <picorbc.h>

extern mrbc_tcb *tcb_sandbox;

void c_sandbox_state(mrb_vm *vm, mrb_value *v, int argc);
void c_sandbox_result(mrb_vm *vm, mrb_value *v, int argc);
void c_invoke_ruby(mrb_vm *vm, mrb_value *v, int argc);

void create_sandbox(void);

#define SANDBOX_INIT() do { \
  mrbc_class *mrbc_class_Keyboard = mrbc_define_class(0, "Keyboard", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Keyboard, "invoke_ruby",    c_invoke_ruby);    \
  mrbc_define_method(0, mrbc_class_Keyboard, "sandbox_state",  c_sandbox_state);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "sandbox_result", c_sandbox_result); \
} while (0)

