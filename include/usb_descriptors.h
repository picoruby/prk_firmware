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

void usb_init(void);

#endif /* USB_DESCRIPTORS_H_ */


