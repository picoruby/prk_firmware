#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

/* TinyUSB */
#include "tusb.h"

enum {
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_RAWHID,
  REPORT_ID_COUNT
};

void prk_init_usb(void);

#endif /* USB_DESCRIPTORS_H_ */

