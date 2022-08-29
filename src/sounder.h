#include <mrubyc.h>

void c_sounder_init(mrb_vm *vm, mrb_value *v, int argc);
void c_sounder_set_tones(mrb_vm *vm, mrb_value *v, int argc);
void c_sounder_start(mrb_vm *vm, mrb_value *v, int argc);

#define SOUNDER_INIT() do {\
  mrbc_class *mrbc_class_Sounder = mrbc_define_class(0, "Sounder", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Sounder, "sounder_init",         c_sounder_init);   \
  mrbc_define_method(0, mrbc_class_Sounder, "sounder_start",        c_sounder_start);  \
  mrbc_define_method(0, mrbc_class_Sounder, "sounder_set_tones", c_sounder_set_tones); \
} while (0)
