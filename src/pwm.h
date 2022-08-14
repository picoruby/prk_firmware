#include <mrubyc.h>

void c_pwm_init(mrb_vm *vm, mrb_value *v, int argc);
void c_pwm_enable(mrb_vm *vm, mrb_value *v, int argc);

#define PWM_INIT() do {\
  mrbc_class *mrbc_class_PWM = mrbc_define_class(0, "PWM", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_PWM, "pwm_init",   c_pwm_init);  \
  mrbc_define_method(0, mrbc_class_PWM, "pwm_enable", c_pwm_enable);\
} while (0)
