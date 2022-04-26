/* mruby/c VM */
#include <mrubyc.h>

#include "hardware/adc.h"

void c_init_adc_table(mrb_vm *vm, mrb_value *v, int argc);
void c_init_joystick(mrb_vm *vm, mrb_value *v, int argc);
void c_report_gamepad(mrb_vm *vm, mrb_value *v, int argc);

void RotaryEncoder_reset(void);

#define GAMEPAD_INIT() do { \
  adc_init(); \
  mrbc_class *mrbc_class_Gamepad = mrbc_define_class(0, "Gamepad", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Gamepad, "init_adc_table", c_init_adc_table); \
  mrbc_define_method(0, mrbc_class_Gamepad, "init_joystick",  c_init_joystick); \
  mrbc_define_method(0, mrbc_class_Gamepad, "report_gamepad", c_report_gamepad); \
} while (0)

