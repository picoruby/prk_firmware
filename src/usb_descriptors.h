/* mruby/c VM */
#include <mrubyc.h>

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

void c_tud_task(mrb_vm *vm, mrb_value *v, int argc);
void c_tud_mounted_q(mrb_vm *vm, mrb_value *v, int argc);

void c_report_hid(mrb_vm *vm, mrb_value *v, int argc);
void c_report_raw_hid(mrb_vm *vm, mrb_value *v, int argc);
void c_raw_hid_report_received_Q(mrb_vm *vm, mrb_value *v, int argc);
void c_set_raw_hid_report_read(mrb_vm *vm, mrb_value *v, int argc);
void c_get_last_received_raw_hid_report(mrb_vm *vm, mrb_value *v, int argc);

#define VIA_INIT() do {\
  mrbc_define_method(0, mrbc_class_object, "report_raw_hid", c_report_raw_hid);\
  mrbc_define_method(0, mrbc_class_object, "raw_hid_report_received?", c_raw_hid_report_received_Q);\
  mrbc_define_method(0, mrbc_class_object, "set_raw_hid_report_read", c_set_raw_hid_report_read);\
  mrbc_define_method(0, mrbc_class_object, "get_last_received_raw_hid_report", c_get_last_received_raw_hid_report);\
} while(0)

#define USB_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "tud_task",     c_tud_task);      \
  mrbc_define_method(0, mrbc_class_object, "tud_mounted?", c_tud_mounted_q); \
  mrbc_define_method(0, mrbc_class_object, "report_hid",   c_report_hid);    \
  VIA_INIT();\
} while (0)

#endif /* USB_DESCRIPTORS_H_ */


