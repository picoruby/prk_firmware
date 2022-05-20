/* mruby/c VM */
#include <mrubyc.h>

#include "hardware/adc.h"

void joystick_report(void);

void c_reset_axes(mrb_vm *vm, mrb_value *v, int argc);
void c_init_axis_offset(mrb_vm *vm, mrb_value *v, int argc);
void c_set_values(mrb_vm *vm, mrb_value *v, int argc);

void RotaryEncoder_reset(void);

#define JOYSTICK_INIT() do { \
  adc_init(); \
  mrbc_class *mrbc_class_Joystick = mrbc_define_class(0, "Joystick", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Joystick, "reset_axes",        c_reset_axes); \
  mrbc_define_method(0, mrbc_class_Joystick, "init_axis_offset",  c_init_axis_offset); \
  mrbc_define_method(0, mrbc_class_Joystick, "set_values",        c_set_values); \
} while (0)

