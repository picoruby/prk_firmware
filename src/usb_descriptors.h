#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

/* mruby/c VM */
#include <mrubyc.h>

/* TinyUSB */
#include "tusb.h"

extern tusb_desc_device_t desc_device;

extern char *iProduct;

extern char const *string_desc_arr[];

#define STRING_DESC_ARR_SIZE 6

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
void c_raw_hid_report_received_q(mrb_vm *vm, mrb_value *v, int argc);
void c_get_last_received_raw_hid_report(mrb_vm *vm, mrb_value *v, int argc);

void c_Keyboard_start_observing_output_report(mrb_vm *vm, mrb_value *v, int argc);
void c_Keyboard_stop_observing_output_report(mrb_vm *vm, mrb_value *v, int argc);
void c_Keyboard_output_report(mrb_vm *vm, mrb_value *v, int argc);

#define VIA_INIT() do {\
  mrbc_define_method(0, mrbc_class_object, "report_raw_hid", c_report_raw_hid);\
  mrbc_define_method(0, mrbc_class_object, "raw_hid_report_received?", c_raw_hid_report_received_q);\
  mrbc_define_method(0, mrbc_class_object, "get_last_received_raw_hid_report", c_get_last_received_raw_hid_report);\
} while(0)

#define USB_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "tud_task",     c_tud_task);      \
  mrbc_define_method(0, mrbc_class_object, "tud_mounted?", c_tud_mounted_q); \
  mrbc_define_method(0, mrbc_class_object, "report_hid",   c_report_hid);    \
  mrbc_define_method(0, mrbc_class_Keyboard, "stop_observing_output_report", \
                                   c_Keyboard_stop_observing_output_report);   \
  mrbc_define_method(0, mrbc_class_Keyboard, "start_observing_output_report",\
                                   c_Keyboard_start_observing_output_report);  \
  mrbc_define_method(0, mrbc_class_Keyboard, "output_report",    \
                                   c_Keyboard_output_report);                  \
  VIA_INIT();\
} while (0)

#endif /* USB_DESCRIPTORS_H_ */


