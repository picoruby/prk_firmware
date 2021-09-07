/* mruby/c VM */
#include <mrubyc.h>

void msc_init(void);
void c_cdc_task(mrb_vm *vm, mrb_value *v, int argc);

#define MSC_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "cdc_task",  c_cdc_task);  \
} while (0)
