#include "tusb.h"

#include "usb_descriptors.h"
#include "raw_hid.h"
#include "joystick.h"

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  /* desc_device is defined in main.c */
  return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#ifdef PRK_NO_MSC
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
#else
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MSC_DESC_LEN + TUD_HID_INOUT_DESC_LEN)
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

#define EPNUM_HID_IN    0x04
#define EPNUM_HID_OUT   0x84

uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(REPORT_ID_KEYBOARD         )),
  TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(REPORT_ID_MOUSE            )),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL )),
  TUD_HID_REPORT_DESC_GAMEPAD ( HID_REPORT_ID(REPORT_ID_GAMEPAD          )),
  RAW_HID_REPORT_DESC(          HID_REPORT_ID(REPORT_ID_RAWHID           )),
};

enum
{
  ITF_NUM_CDC = 0,
  ITF_NUM_CDC_DATA,
  ITF_NUM_HID,
#ifndef PRK_NO_MSC
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

#ifndef PRK_NO_MSC
  // Interface number, string index, EP Out & EP In address, EP size
  TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
#endif

  // Interface number, string index, protocol, report descriptor len, EP In address, size & polling interval
  TUD_HID_INOUT_DESCRIPTOR(ITF_NUM_HID, 0, 0, sizeof(desc_hid_report), EPNUM_HID_OUT, EPNUM_HID_IN, 64, 0x08),
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

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const*
tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if ( !(index < STRING_DESC_ARR_SIZE) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}

const uint16_t string_desc_product[] = { // Index: 1
  16 | (3 << 8),
  'P', 'R', 'K', 'f', 'i', 'r', 'm'
};

uint8_t raw_hid_last_received_report[REPORT_RAW_MAX_LEN];
uint8_t raw_hid_last_received_report_length;
bool raw_hid_report_received = false;
bool observing_output_report = false;
uint8_t keyboard_output_report = 0;

uint8_t const *
tud_hid_descriptor_report_cb(uint8_t instance) {
    return desc_hid_report;
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
    case REPORT_ID_RAWHID:
      memcpy(raw_hid_last_received_report, buffer, bufsize);
      raw_hid_last_received_report_length = bufsize;
      raw_hid_report_received = true;
      break;
    case REPORT_ID_KEYBOARD:
      if (observing_output_report) {
        keyboard_output_report = buffer[0];
      }
      break;
    default:
      break;
  }
}

static uint8_t keyboard_modifier = 0;
static uint8_t *keyboard_keycodes = NULL;
static uint16_t consumer_keycode = 0;
static uint32_t joystick_buttons = 0;
static uint8_t joystick_hat = 0;

static void
send_hid_report(uint8_t report_id)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return;

  switch(report_id)
  {
    case REPORT_ID_KEYBOARD: {
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, keyboard_modifier, keyboard_keycodes);
    }
    break;

    case REPORT_ID_MOUSE: {
      int8_t const delta = 0;
      tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);
    }
    break;

    case REPORT_ID_CONSUMER_CONTROL: {
      static int16_t prev_keycode = 0;
      if (prev_keycode == consumer_keycode) {
        consumer_keycode = 0;
      } else {
        prev_keycode = consumer_keycode;
      }
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &consumer_keycode, 2);
    }
    break;

    case REPORT_ID_GAMEPAD: {
      joystick_report_hid(joystick_buttons, joystick_hat);
    }
    break;

//    case REPORT_ID_RAWHID: {
//      tud_hid_report(REPORT_ID_RAWHID, raw_c_data, raw_len);
//    }
//    break;

    default: break;
  }
}

void
tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;
  uint8_t next_report_id = report[0] + 1;
  if (next_report_id < REPORT_ID_COUNT - 1) {
    //                 ^^^^^^^^^^^^^^^^^^^ skip REPORT_ID_RAWHID
    send_hid_report(next_report_id);
  }
}

//--------------------------------------------------------------------+
// Ruby methods
//--------------------------------------------------------------------+

void
c_Keyboard_start_observing_output_report(mrb_vm *vm, mrb_value *v, int argc) {
  observing_output_report = true;
}

void
c_Keyboard_stop_observing_output_report(mrb_vm *vm, mrb_value *v, int argc) {
  observing_output_report = false;
}

void
c_Keyboard_output_report(mrb_vm *vm, mrb_value *v, int argc) {
  SET_INT_RETURN(keyboard_output_report);
}

void
c_raw_hid_report_received_q(mrb_vm *vm, mrb_value *v, int argc) {
  if(raw_hid_report_received) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

void
c_get_last_received_raw_hid_report(mrb_vm *vm, mrb_value *v, int argc) {
  mrbc_value rb_val_array = mrbc_array_new(vm, REPORT_RAW_MAX_LEN);
  mrbc_array *rb_array = rb_val_array.array;

  rb_array->n_stored = raw_hid_last_received_report_length;
  for(uint8_t i=0; i<raw_hid_last_received_report_length && i<REPORT_RAW_MAX_LEN; i++) {
    mrbc_set_integer( (rb_array->data)+i, raw_hid_last_received_report[i] );
  }
  raw_hid_report_received = false;

  SET_RETURN(rb_val_array);
}

void
c_tud_task(mrb_vm *vm, mrb_value *v, int argc)
{
  tud_task();
}

bool
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
  if (tud_hid_ready()) {
    return tud_hid_report(REPORT_ID_RAWHID, data, len);
  } else {
    return false;
  }
}

void
c_report_raw_hid(mrb_vm *vm, mrb_value *v, int argc) {
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

void
c_Keyboard_hid_task(mrb_vm *vm, mrb_value *v, int argc)
{
  keyboard_modifier = (uint8_t)GET_INT_ARG(1);
  keyboard_keycodes = GET_STRING_ARG(2);
  consumer_keycode  = (uint16_t)GET_INT_ARG(3);
  joystick_buttons  = (uint32_t)GET_INT_ARG(4);
  joystick_hat      = (uint8_t)GET_INT_ARG(5);

  if (tud_suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    send_hid_report(REPORT_ID_KEYBOARD);
  }
}

void
c_tud_mounted_q(mrb_vm *vm, mrb_value *v, int argc)
{
  if (tud_mounted()) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}
