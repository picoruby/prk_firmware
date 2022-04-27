/* mruby/c VM */
#include <mrubyc.h>

#include "hardware/adc.h"

void gamepad_report(void);

void c_reset_axes(mrb_vm *vm, mrb_value *v, int argc);
void c_init_axis_offset(mrb_vm *vm, mrb_value *v, int argc);
void c_set_values(mrb_vm *vm, mrb_value *v, int argc);

void RotaryEncoder_reset(void);

#define GAMEPAD_INIT() do { \
  adc_init(); \
  mrbc_class *mrbc_class_Gamepad = mrbc_define_class(0, "Gamepad", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Gamepad, "reset_axes",        c_reset_axes); \
  mrbc_define_method(0, mrbc_class_Gamepad, "init_axis_offset",  c_init_axis_offset); \
  mrbc_define_method(0, mrbc_class_Gamepad, "set_values",        c_set_values); \
} while (0)

