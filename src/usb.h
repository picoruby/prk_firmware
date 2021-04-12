#include "tusb.h"
#include "usb_descriptors.h"
#include "bsp/board.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/scb.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"

/* mruby/c VM */
#include <mrubyc.h>

void c_tud_task(mrb_vm *vm, mrb_value *v, int argc);

void c_report_hid(mrb_vm *vm, mrb_value *v, int argc);

void c_tud_mounted_q(mrb_vm *vm, mrb_value *v, int argc);

#define USB_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "tud_task",      c_tud_task);      \
  mrbc_define_method(0, mrbc_class_object, "report_hid",    c_report_hid);    \
  mrbc_define_method(0, mrbc_class_object, "tud_mounted?",  c_tud_mounted_q); \
} while (0)

