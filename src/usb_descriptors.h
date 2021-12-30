/* mruby/c VM */
#include <mrubyc.h>
#include "raw_hid.h"

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

enum {
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_RAWHID,
  REPORT_ID_COUNT
};

void report_raw_hid(uint8_t* data, uint8_t len);

void c_tud_task(mrb_vm *vm, mrb_value *v, int argc);
void c_report_hid(mrb_vm *vm, mrb_value *v, int argc);
void c_start_via(mrb_vm *vm, mrb_value *v, int argc);
void c_tud_mounted_q(mrb_vm *vm, mrb_value *v, int argc);

#define USB_INIT() do { \
  mrbc_define_method(0, mrbc_class_object,   "tud_task",     c_tud_task);      \
  mrbc_define_method(0, mrbc_class_object,   "tud_mounted?", c_tud_mounted_q); \
  mrbc_class *mrbc_class_Keyboard = mrbc_define_class(0, "Keyboard", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Keyboard, "report_hid",   c_report_hid);    \
  mrbc_define_method(0, mrbc_class_Keyboard, "start_via", c_start_via);\
  mrbc_define_method(0, mrbc_class_Keyboard, "get_keycode_via", c_get_keycode_via);\
  mrbc_define_method(0, mrbc_class_Keyboard, "via_keymap_updated?", c_via_keymap_updated);\
  mrbc_define_method(0, mrbc_class_Keyboard, "set_via_keymap_updated?", c_set_via_keymap_updated);\
} while (0)

#endif /* USB_DESCRIPTORS_H_ */


