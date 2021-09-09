/* mruby/c VM */
#include <mrubyc.h>

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

enum {
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_COUNT
};

void c_tud_task(mrb_vm *vm, mrb_value *v, int argc);
void c_report_hid(mrb_vm *vm, mrb_value *v, int argc);
void c_tud_mounted_q(mrb_vm *vm, mrb_value *v, int argc);

#define TUD_INIT() do { \
  mrbc_define_method(0, mrbc_class_object,   "tud_task",     c_tud_task);      \
  mrbc_define_method(0, mrbc_class_object,   "tud_mounted?", c_tud_mounted_q); \
  mrbc_class *mrbc_class_Keyboard = mrbc_define_class(0, "Keyboard", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Keyboard, "report_hid",   c_report_hid);    \
} while (0)

#endif /* USB_DESCRIPTORS_H_ */


