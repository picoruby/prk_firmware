#ifndef RAW_HID_H_
#define RAW_HID_H_

#include "hid.h"

#define REPORT_RAW_MAX_LEN 0x20
#define RAW_HID_REPORT_DESC(...) \
    HID_USAGE_PAGE_N(0xFF60,2),\
    HID_USAGE(0x61), \
    HID_COLLECTION(HID_COLLECTION_APPLICATION), \
    __VA_ARGS__ \
    HID_USAGE(0x62), \
    HID_LOGICAL_MIN(0), \
    HID_LOGICAL_MAX_N(0x00FF, 2), \
    HID_REPORT_COUNT(REPORT_RAW_MAX_LEN), \
    HID_REPORT_SIZE(8), \
    HID_INPUT( HID_DATA | HID_VARIABLE | HID_ABSOLUTE), \
    \
    HID_USAGE(0x63), \
    HID_LOGICAL_MIN(0), \
    HID_LOGICAL_MAX_N(0x00FF, 2), \
    HID_REPORT_COUNT(REPORT_RAW_MAX_LEN), \
    HID_REPORT_SIZE(8), \
    HID_OUTPUT( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_VOLATILE), \
    HID_COLLECTION_END

#define VIA_PROTOCOL_VERSION 0x0009
#define KEYMAP_SIZE_BYTES 40

typedef uint32_t matrix_row_t;

typedef struct {
    uint8_t row_count;
    uint8_t col_count;
} via_keyboard_t;

enum {
    id_get_protocol_version = 0x01,
    id_get_keyboard_value,
    id_set_keyboard_value,
    id_dynamic_keymap_get_keycode,
    id_dynamic_keymap_set_keycode,
    id_dynamic_keymap_reset,
    id_lighting_set_value,
    id_lighting_get_value,
    id_lighting_save,
    id_eeprom_reset,
    id_bootloader_jump,
    id_dynamic_keymap_macro_get_count,
    id_dynamic_keymap_macro_get_buffer_size,
    id_dynamic_keymap_macro_get_buffer,
    id_dynamic_keymap_macro_set_buffer,
    id_dynamic_keymap_macro_reset,
    id_dynamic_keymap_get_layer_count,
    id_dynamic_keymap_get_buffer,
    id_dynamic_keymap_set_buffer,
    id_unhandled = 0xFF,
};

enum {
    id_uptime = 0x01,
    id_layout_options,
    id_switch_matrix_state,
};

extern via_keyboard_t via_setting;
extern uint8_t eeprom[KEYMAP_SIZE_BYTES];

void c_start_via(mrb_vm *vm, mrb_value *v, int argc);
void raw_hid_receive(uint8_t *data, uint8_t length);

#endif