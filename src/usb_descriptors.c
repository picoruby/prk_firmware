#include <mrubyc.h>
#include "hardware/structs/scb.h"

#include "../include/usb_descriptors.h"
#include "../include/raw_hid.h"
#include "../include/joystick.h"
#include "../include/version.h"

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t desc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,
  // Use Interface Association Descriptor (IAD) for CDC
  // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
  .bDeviceClass       = TUSB_CLASS_MISC,
  .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
  .bDeviceProtocol    = MISC_PROTOCOL_IAD,
  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
  /*
   * VID and PID from USB-IDs-for-free.txt
   * https://github.com/obdev/v-usb/blob/releases/20121206/usbdrv/USB-IDs-for-free.txt#L128
   */
  .idVendor           = 0x16c0,
  .idProduct          = 0x27db,
  .bcdDevice          = 0x0100,
  .iManufacturer      = 0x01,
  .iProduct           = 0x02,
  .iSerialNumber      = 0x03,
  .bNumConfigurations = 0x01
};

#define STRING_DESC_ARR_SIZE 6

#define PRK_CONF_SIZE 64

static char prk_conf_name[PRK_CONF_SIZE - 14]; // len("0x1111:0x1111:") == 14

static char const *string_desc_arr[STRING_DESC_ARR_SIZE] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "PRK Firmware developers",     // 1: Manufacturer
  prk_conf_name,                 // 2: Product
  PRK_SERIAL,                    // 3: Serial
  "PRK CDC",                     // 4: CDC Interface
  "PRK MSC",                     // 5: MSC Interface
};

// Invoked when received GET DEVICE DESCRIPTOR
uint8_t const *
tud_descriptor_device_cb(void)
{
  return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#ifdef PICORUBY_NO_MSC
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_INOUT_DESC_LEN + TUD_HID_DESC_LEN)
#else
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MSC_DESC_LEN + TUD_HID_INOUT_DESC_LEN + TUD_HID_DESC_LEN)
#endif

#if CFG_TUSB_MCU == OPT_MCU_LPC175X_6X || CFG_TUSB_MCU == OPT_MCU_LPC177X_8X || CFG_TUSB_MCU == OPT_MCU_LPC40XX
  // LPC 17xx and 40xx endpoint type (bulk/interrupt/iso) are fixed by its number
  // 0 control, 1 In, 2 Bulk, 3 Iso, 4 In, 5 Bulk etc ...
  #define EPNUM_CDC_NOTIF   0x81
  #define EPNUM_CDC_OUT     0x02
  #define EPNUM_CDC_IN      0x82

  #define EPNUM_MSC_OUT     0x05
  #define EPNUM_MSC_IN      0x85

#elif CFG_TUSB_MCU == OPT_MCU_SAMG  || CFG_TUSB_MCU ==  OPT_MCU_SAMX7X
  // SAMG & SAME70 don't support a same endpoint number with different direction IN and OUT
  //    e.g EP1 OUT & EP1 IN cannot exist together
  #define EPNUM_CDC_NOTIF   0x81
  #define EPNUM_CDC_OUT     0x02
  #define EPNUM_CDC_IN      0x83

  #define EPNUM_MSC_OUT     0x04
  #define EPNUM_MSC_IN      0x85

#elif CFG_TUSB_MCU == OPT_MCU_CXD56
  // CXD56 doesn't support a same endpoint number with different direction IN and OUT
  //    e.g EP1 OUT & EP1 IN cannot exist together
  // CXD56 USB driver has fixed endpoint type (bulk/interrupt/iso) and direction (IN/OUT) by its number
  // 0 control (IN/OUT), 1 Bulk (IN), 2 Bulk (OUT), 3 In (IN), 4 Bulk (IN), 5 Bulk (OUT), 6 In (IN)
  #define EPNUM_CDC_NOTIF   0x83
  #define EPNUM_CDC_OUT     0x02
  #define EPNUM_CDC_IN      0x81

  #define EPNUM_MSC_OUT     0x05
  #define EPNUM_MSC_IN      0x84

#else
  #define EPNUM_CDC_NOTIF   0x81
  #define EPNUM_CDC_OUT     0x02
  #define EPNUM_CDC_IN      0x82

  #define EPNUM_MSC_OUT     0x03
  #define EPNUM_MSC_IN      0x83

#endif

#define EPNUM_HID_OUT  0x04
#define EPNUM_HID_IN   0x84
#define EPNUM_JOYSTICK_IN 0x85

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD         )),
  TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(REPORT_ID_MOUSE            )),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL )),
  RAW_HID_REPORT_DESC(          HID_REPORT_ID(REPORT_ID_RAWHID           )),
};

uint8_t const desc_joystick_report[] =
{
  TUD_HID_REPORT_DESC_GAMEPAD ( HID_REPORT_ID(REPORT_ID_GAMEPAD          )),
};

enum
{
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  ITF_NUM_HID,
  ITF_NUM_JOYSTICK,
#ifndef PICORUBY_NO_MSC
  ITF_NUM_MSC,
#endif
  ITF_NUM_TOTAL
};

uint8_t const desc_fs_configuration[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),

  // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
  TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),

#ifndef PICORUBY_NO_MSC
  // Interface number, string index, EP Out & EP In address, EP size
  TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
#endif

  // Interface number, string index, protocol, report descriptor len, EP In address, EP Out address, size & polling interval
  TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, 0, sizeof(desc_hid_report), EPNUM_HID_IN, EPNUM_HID_OUT, 64, 0x08 ),

  // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_JOYSTICK, 0, 0, sizeof(desc_joystick_report), EPNUM_JOYSTICK_IN, 20, 0x08 )
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const*
tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return desc_fs_configuration;
}

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const*
tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;
  static uint16_t _desc_str[32];
  uint8_t chr_count;
  if (index == 0) {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  } else {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
    if (!(index < STRING_DESC_ARR_SIZE)) return NULL;
    const char* str = string_desc_arr[index];
    // Cap at max char
    chr_count = strlen(str);
    if (chr_count > 31) chr_count = 31;
    // Convert ASCII string into UTF-16
    for (uint8_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
  }
  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
  return _desc_str;
}

uint8_t raw_hid_last_received_report[REPORT_RAW_MAX_LEN];
uint8_t raw_hid_last_received_report_length;
bool raw_hid_report_received = false;
bool observing_output_report = false;
uint8_t keyboard_output_report = 0;

uint8_t const *
tud_hid_descriptor_report_cb(uint8_t instance) {
  switch(instance) {
    case 0: {
      return desc_hid_report;
    }
    break;

    case 1: {
      return desc_joystick_report;
    }
    break;

    default: {
      return desc_hid_report;
    }
    break;
  }
}

uint16_t
tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    return 0;
}

void
tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
  if (report_type == HID_REPORT_TYPE_INVALID) {
    report_id = buffer[0];
    buffer++;
    bufsize--;
  } else if(report_type != HID_REPORT_TYPE_OUTPUT && report_type != HID_REPORT_TYPE_FEATURE) {
    return;
  }

  if (bufsize < 1) return;

  switch (report_id) {
    case REPORT_ID_KEYBOARD:
      if (observing_output_report) {
        keyboard_output_report = buffer[0];
      }
      break;
    case REPORT_ID_RAWHID:
      memcpy(raw_hid_last_received_report, buffer, bufsize);
      raw_hid_last_received_report_length = bufsize;
      raw_hid_report_received = true;
      break;
    
    default:
      break;
  }
}

void
tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
  (void) instance;
}

#define HID0_REPORT_ID_BITS ( \
    (1<<REPORT_ID_KEYBOARD) | (1<<REPORT_ID_MOUSE) | \
    (1<<REPORT_ID_CONSUMER_CONTROL) | (1<<REPORT_ID_RAWHID) )

static uint8_t input_updated_bitmap = 0;
static uint8_t keyboard_modifier = 0;
static uint8_t keyboard_keycodes[6] = {0, 0, 0, 0, 0, 0};
static uint16_t consumer_keycode = 0;
static uint32_t joystick_buttons = 0;
static uint8_t joystick_hat = 0;
static bool via_active = false;

typedef struct mouse_values {
  uint8_t buttons;
  int8_t x;
  int8_t y;
  int8_t vertical;
  int8_t horizontal;
} MouseValues;
static MouseValues mouse;

static void
send_hid_report()
{
  // skip if report is not updated
  if ( input_updated_bitmap ) {
    // skip if hid is not ready yet
    if ( !tud_hid_n_ready(0) ) {
      input_updated_bitmap &= ~(HID0_REPORT_ID_BITS);
    }
    if ( !tud_hid_n_ready(1) ) {
      input_updated_bitmap &= ~(1<<REPORT_ID_GAMEPAD);
    }
  } else {
    return;
  }

  for(uint8_t i = 1; i < 6; i++) {
    if(input_updated_bitmap & (1<<i) ) {
      switch(i)
      {
        case REPORT_ID_KEYBOARD: {
          tud_hid_n_keyboard_report(0, REPORT_ID_KEYBOARD, keyboard_modifier, keyboard_keycodes);
        }
        break;

        case REPORT_ID_MOUSE: {
          tud_hid_n_mouse_report(0, REPORT_ID_MOUSE,
            mouse.buttons,
            mouse.x,
            mouse.y,
            mouse.vertical,
            mouse.horizontal
          );
          memset(&mouse, 0, sizeof(MouseValues));
        }
        break;

        case REPORT_ID_CONSUMER_CONTROL: {
          if(!via_active) {
            tud_hid_n_report(0, REPORT_ID_CONSUMER_CONTROL, &consumer_keycode, 2);
          }
        }
        break;

        case REPORT_ID_GAMEPAD: {
          joystick_report_hid(joystick_buttons, joystick_hat);
        }
        break;

        default: break;
      }
      return;
    }
  }
}

void
tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;
  
  input_updated_bitmap &= ~(1<<report[0]); // report[0] is report ID
  send_hid_report();
}

//--------------------------------------------------------------------+
// Ruby methods
//--------------------------------------------------------------------+

static void
c_start_observing_output_report(mrbc_vm *vm, mrbc_value *v, int argc) {
  observing_output_report = true;
}

static void
c_stop_observing_output_report(mrbc_vm *vm, mrbc_value *v, int argc) {
  observing_output_report = false;
}

static void
c_output_report(mrbc_vm *vm, mrbc_value *v, int argc) {
  SET_INT_RETURN(keyboard_output_report);
}

static void
c_hid_task(mrbc_vm *vm, mrbc_value *v, int argc)
{
  if(keyboard_modifier != GET_INT_ARG(1)) {
    keyboard_modifier = (uint8_t)GET_INT_ARG(1);
    input_updated_bitmap |= 1<<REPORT_ID_KEYBOARD;
  }

  mrbc_array keycodes = *(GET_ARY_ARG(2).array);
  char keycodes_join[6];
  for (int i = 0; i < 6; i++) {
    if (i < keycodes.n_stored) {
      keycodes_join[i] = mrbc_integer(keycodes.data[i]);
    } else {
      keycodes_join[i] = 0;
    }
  }
  if(memcmp(keyboard_keycodes, keycodes_join, 6)) {
    memcpy(keyboard_keycodes, keycodes_join, 6);
    input_updated_bitmap |= 1<<REPORT_ID_KEYBOARD;
  }

  if(consumer_keycode != GET_INT_ARG(3)) {
    consumer_keycode = (uint16_t)GET_INT_ARG(3);
    input_updated_bitmap |= 1<<REPORT_ID_CONSUMER_CONTROL;
  }

  static bool mouse_zero_report = false;
  if (mouse.x != 0 ||
      mouse.y != 0 ||
      0 < mouse.buttons ||
      mouse.vertical != 0 ||
      mouse.horizontal != 0)
  {
    input_updated_bitmap |= 1<<REPORT_ID_MOUSE;
    mouse_zero_report = false;
  } else if (!mouse_zero_report) {
    input_updated_bitmap |= 1<<REPORT_ID_MOUSE;
    mouse_zero_report = true;
  }

  if (consumer_keycode != 0) {
    via_active = false;
  }

  if (tud_suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }

  send_hid_report();
}

static void
c_raw_hid_report_received_q(mrbc_vm *vm, mrbc_value *v, int argc) {
  if(raw_hid_report_received) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

static void
c_get_last_received_raw_hid_report(mrbc_vm *vm, mrbc_value *v, int argc) {
  mrbc_value rb_val_array = mrbc_array_new(vm, REPORT_RAW_MAX_LEN);
  mrbc_array *rb_array = rb_val_array.array;

  rb_array->n_stored = raw_hid_last_received_report_length;
  for(uint8_t i=0; i<raw_hid_last_received_report_length && i<REPORT_RAW_MAX_LEN; i++) {
    mrbc_set_integer( (rb_array->data)+i, raw_hid_last_received_report[i] );
  }
  raw_hid_report_received = false;

  SET_RETURN(rb_val_array);
}

static void
c_tud_task(mrbc_vm *vm, mrbc_value *v, int argc)
{
  tud_task();
}

static bool
report_raw_hid(uint8_t* data, uint8_t len)
{
  bool ret;
  // Remote wakeup
  if (tud_suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }
  /*------------- RAW HID -------------*/
  if (tud_hid_n_ready(0)) {
    via_active = true;
    return tud_hid_n_report(0, REPORT_ID_RAWHID, data, len);
  } else {
    return false;
  }
}

static void
c_report_raw_hid(mrbc_vm *vm, mrbc_value *v, int argc) {
  mrbc_array rb_ary = *( GET_ARY_ARG(1).array );
  uint8_t c_data[REPORT_RAW_MAX_LEN];
  uint8_t len = REPORT_RAW_MAX_LEN;

  memset(c_data, 0, REPORT_RAW_MAX_LEN);
  if(GET_ARY_ARG(1).tt == MRBC_TT_ARRAY) {
    if(rb_ary.n_stored<len) {
      len = rb_ary.n_stored;
    }
    for(uint8_t i=0; i<len; i++) {
      c_data[i] = mrbc_integer(rb_ary.data[i]);
    }
  }

  if( report_raw_hid(c_data, len) ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

static void
c_tud_mounted_q(mrbc_vm *vm, mrbc_value *v, int argc)
{
  if (tud_mounted()) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

static void
c_merge_joystick_report(mrbc_vm *vm, mrbc_value *v, int argc)
{
  joystick_buttons = (uint32_t)GET_INT_ARG(1);
  joystick_hat     = (uint8_t)GET_INT_ARG(2);
  input_updated_bitmap |= 1<<REPORT_ID_GAMEPAD;
  SET_NIL_RETURN();
}


static void
c_merge_mouse_report(mrbc_vm *vm, mrbc_value *v, int argc)
{
  mouse.buttons    |= GET_INT_ARG(1);
  mouse.x          += GET_INT_ARG(2);
  mouse.y          += GET_INT_ARG(3);
  mouse.horizontal += GET_INT_ARG(4);
  mouse.vertical   += GET_INT_ARG(5);
  SET_NIL_RETURN();
}

//--------------------------------------------------------------------+
// String Descriptors
// Note: Tentative and dirty implementation until getting rid of VIA
//--------------------------------------------------------------------+
#include "../include/msc_disk.h"
static void
c_save_prk_conf(mrbc_vm *vm, mrbc_value *v, int argc)
{
  uint8_t buff[SECTOR_SIZE] = {0};
  memcpy(buff, (const uint8_t *)GET_STRING_ARG(1), strlen((const uint8_t *)GET_STRING_ARG(1)));
  if (strncmp((const char *)buff,
              (const char *)(FLASH_MMAP_ADDR - SECTOR_SIZE),
              PRK_CONF_SIZE) == 0)
  {
    SET_FALSE_RETURN();
    /* Do nothing if no need to change */
    return;
  }

  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(
    (uint32_t)(FLASH_TARGET_OFFSET - SECTOR_SIZE),
    (size_t)(SECTOR_SIZE)
  );
  flash_range_program(
    (uint32_t)(FLASH_TARGET_OFFSET - SECTOR_SIZE),
    (const uint8_t *)buff,
    (size_t)(SECTOR_SIZE)
  );
  restore_interrupts(ints);
  SET_TRUE_RETURN();
}

static void
load_prk_conf(char *prk_conf)
{
  memcpy(prk_conf, (uint8_t *)(FLASH_MMAP_ADDR - SECTOR_SIZE), PRK_CONF_SIZE);
}

static void
c_prk_conf(mrbc_vm *vm, mrbc_value *v, int argc)
{
  char prk_conf[PRK_CONF_SIZE];
  load_prk_conf(prk_conf);
  mrbc_value value = mrbc_string_new_cstr(vm, prk_conf);
  SET_RETURN(value);
}

#include <stdlib.h>
void
prk_init_USB(void)
{
  strcpy(prk_conf_name, "Default VID/PID");
  char prk_conf[PRK_CONF_SIZE] = {0};
  load_prk_conf(prk_conf);
  while (0 < strlen(prk_conf)) {
    char *tok;
    tok = strtok(prk_conf, ":");
    if (!tok) break;
    desc_device.idVendor = (uint16_t)strtol(tok, NULL, 16);
    tok = strtok(NULL, ":");
    if (!tok) break;
    desc_device.idProduct = (uint16_t)strtol(tok, NULL, 16);
    tok = strtok(NULL, ":");
    if (!tok) break;
    strcpy(prk_conf_name, tok);
    break;
  }

  mrbc_class *mrbc_class_USB = mrbc_define_class(0, "USB", mrbc_class_object);

  mrbc_define_method(0, mrbc_class_USB, "save_prk_conf", c_save_prk_conf);
  mrbc_define_method(0, mrbc_class_USB, "prk_conf", c_prk_conf);
  mrbc_define_method(0, mrbc_class_USB, "tud_task", c_tud_task);
  mrbc_define_method(0, mrbc_class_USB, "tud_mounted?", c_tud_mounted_q);
  mrbc_define_method(0, mrbc_class_USB, "hid_task", c_hid_task);

  mrbc_define_method(0, mrbc_class_USB, "merge_joystick_report", c_merge_joystick_report);
  memset(&mouse, 0, sizeof(MouseValues));
  mrbc_define_method(0, mrbc_class_USB, "merge_mouse_report", c_merge_mouse_report);

  /* for Caps lock etc. */
  mrbc_define_method(0, mrbc_class_USB, "stop_observing_output_report", c_stop_observing_output_report);
  mrbc_define_method(0, mrbc_class_USB, "start_observing_output_report", c_start_observing_output_report);
  mrbc_define_method(0, mrbc_class_USB, "output_report", c_output_report);

  /* for VIA */
  mrbc_define_method(0, mrbc_class_USB, "report_raw_hid", c_report_raw_hid);
  mrbc_define_method(0, mrbc_class_USB, "raw_hid_report_received?", c_raw_hid_report_received_q);
  mrbc_define_method(0, mrbc_class_USB, "get_last_received_raw_hid_report", c_get_last_received_raw_hid_report);
}
