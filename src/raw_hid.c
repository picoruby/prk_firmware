#include "usb_descriptors.h"
#include "raw_hid.h"
#include "value.h"
#include "pico/time.h"
#include <mrubyc.h>
#include "hardware/flash.h"
#include "pico/stdlib.h"

#define MAGIC 0xDEADBEEF

via_keyboard_t via_setting = {
    .col_count = 2,
    .row_count = 2,
};

uint8_t eeprom[KEYMAP_SIZE_BYTES];
const uint8_t *via_flash = (const uint8_t *) (XIP_BASE + VIA_KEYMAP_FLASH_OFFSET);
bool keymap_updated = false;
bool keymap_saved = true;

void c_via_keymap_updated(mrb_vm *vm, mrb_value *v, int argc) {
    if(keymap_updated) {
        SET_TRUE_RETURN();
    } else {
        SET_FALSE_RETURN();
    } 
}

void c_set_via_keymap_updated(mrb_vm *vm, mrb_value *v, int argc) {
    keymap_updated = false;
}

void save_keymap(void) {
    if(! keymap_saved) {
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(VIA_KEYMAP_FLASH_OFFSET, (KEYMAP_SIZE_BYTES/FLASH_SECTOR_SIZE+1)*FLASH_SECTOR_SIZE);
        flash_range_program(VIA_KEYMAP_FLASH_OFFSET, eeprom, ( KEYMAP_SIZE_BYTES/FLASH_PAGE_SIZE +1)*FLASH_PAGE_SIZE);
        restore_interrupts(ints);
        keymap_saved = true;
    }
}

uint8_t dynamic_keymap_get_layer_count(void) {
    return KEYMAP_SIZE_BYTES / sizeof(uint16_t) / (via_setting.col_count * via_setting.row_count);
}

void c_get_via_layer_count(mrb_vm *vm, mrb_value *v, int argc) {
    SET_INT_RETURN(dynamic_keymap_get_layer_count());
}

inline uint8_t eeprom_read_byte(uint8_t *address) {
    return *address;
}

void eeprom_update_byte(uint8_t* addr, uint8_t val) {
    *addr = val;
}

inline uint8_t* via_get_eeprom_addr(void) {
    return eeprom;
}

uint8_t* dynamic_keymap_key_to_eeprom_address(uint8_t layer, uint8_t row, uint8_t column) {
    return via_get_eeprom_addr() + ( layer * via_setting.row_count * via_setting.col_count * 2 ) + (row * via_setting.col_count * 2) + (column * 2);
}

uint16_t dynamic_keymap_get_keycode(uint8_t layer, uint8_t row, uint8_t column) {
    uint8_t *address = dynamic_keymap_key_to_eeprom_address(layer, row, column);
    // Big endian, so we can read/write EEPROM directly from host if we want
    uint16_t keycode = eeprom_read_byte(address) << 8;
    keycode |= eeprom_read_byte(address + 1);
    return keycode;
}

void c_get_keycode_via(mrb_vm *vm, mrb_value *v, int argc) {
    uint8_t layer = GET_INT_ARG(1);
    uint8_t row = GET_INT_ARG(2);
    uint8_t col = GET_INT_ARG(3);

    uint16_t keycode = dynamic_keymap_get_keycode(layer, row, col);

    SET_INT_RETURN(keycode);
}

void dynamic_keymap_set_keycode(uint8_t layer, uint8_t row, uint8_t column, uint16_t keycode) {
    uint8_t *address = dynamic_keymap_key_to_eeprom_address(layer, row, column);
    // Big endian, so we can read/write EEPROM directly from host if we want
    eeprom_update_byte(address, (uint8_t)(keycode >> 8));
    eeprom_update_byte(address + 1, (uint8_t)(keycode & 0xFF));
}

bool dynamic_keymap_initialized() {
    for(uint8_t i=0; i<4; i++) {
        if( *(via_flash-FLASH_PAGE_SIZE+i) != (0x00FF & (MAGIC>>8*(3-i))) ) {
            return false;
        }
    }
    return true;
}

void c_via_initialized(mrb_vm *vm, mrb_value *v, int argc) {
    if( dynamic_keymap_initialized() ) {
        SET_TRUE_RETURN();
    } else { 
        SET_FALSE_RETURN();
    } 
}

void c_via_set_initialized(mrb_vm *vm, mrb_value *v, int argc) {
    uint8_t data[FLASH_PAGE_SIZE];
    memset(data, 0, FLASH_PAGE_SIZE);

    for(uint8_t i=0; i<4; i++) {
        *(data+i) = 0xFF & ( MAGIC>>(8*(3-i)) );
    }

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(VIA_KEYMAP_FLASH_OFFSET-FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
    flash_range_program(VIA_KEYMAP_FLASH_OFFSET-FLASH_PAGE_SIZE, data, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

void dynamic_keymap_get_buffer(uint16_t offset, uint16_t size, uint8_t *data) {
    uint16_t dynamic_keymap_eeprom_size = dynamic_keymap_get_layer_count() * via_setting.row_count * via_setting.col_count * 2;
    uint8_t *source                     = (via_get_eeprom_addr() + offset);
    uint8_t *target                     = data;
    for (uint16_t i = 0; i < size; i++) {
        if (offset + i < dynamic_keymap_eeprom_size) {
            *target = eeprom_read_byte(source);
        } else {
            *target = 0x00;
        }
        source++;
        target++;
    }
}

void dynamic_keymap_set_buffer(uint16_t offset, uint16_t size, uint8_t *data) {
    uint16_t dynamic_keymap_eeprom_size = dynamic_keymap_get_layer_count() * via_setting.row_count * via_setting.col_count * 2;
    uint8_t *target                     = (via_get_eeprom_addr() + offset);
    uint8_t *source                     = data;
    for (uint16_t i = 0; i < size; i++) {
        if (offset + i < dynamic_keymap_eeprom_size) {
            eeprom_update_byte(target, *source);
        }
        source++;
        target++;
    }
}

void c_via_set_key(mrb_vm *vm, mrb_value *v, int argc) {
    uint8_t layer = GET_INT_ARG(1);
    uint8_t row = GET_INT_ARG(2);
    uint8_t col = GET_INT_ARG(3);
    uint16_t keycode = GET_INT_ARG(4);
    dynamic_keymap_set_keycode(layer, row, col, keycode);
}

void c_start_via(mrb_vm *vm, mrb_value *v, int argc) {
    via_setting.row_count = GET_INT_ARG(1);
    via_setting.col_count = GET_INT_ARG(2);
    
    if(dynamic_keymap_initialized()) {
        memcpy(eeprom, via_flash, KEYMAP_SIZE_BYTES);
    } else {
        memset(eeprom, 0, KEYMAP_SIZE_BYTES);
    }
}

void raw_hid_send(uint8_t *data, uint8_t len) {
    report_raw_hid(data, len);
}

// This is generalized so the layout options EEPROM usage can be
// variable, between 1 and 4 bytes.
/*
static uint32_t via_get_layout_options(void) {
    uint32_t value = 0;
    // Start at the most significant byte
    void *source = (void *)(VIA_EEPROM_LAYOUT_OPTIONS_ADDR);
    for (uint8_t i = 0; i < VIA_EEPROM_LAYOUT_OPTIONS_SIZE; i++) {
        value = value << 8;
        value |= eeprom_read_byte(source);
        source++;
    }
    return value;
}

static void via_set_layout_options(uint32_t value) {
    // Start at the least significant byte
    void *target = (void *)(VIA_EEPROM_LAYOUT_OPTIONS_ADDR + VIA_EEPROM_LAYOUT_OPTIONS_SIZE - 1);
    for (uint8_t i = 0; i < VIA_EEPROM_LAYOUT_OPTIONS_SIZE; i++) {
        eeprom_update_byte(target, value & 0xFF);
        value = value >> 8;
        target--;
    }
    keymap_updated = true;
}
*/
static void raw_hid_receive_kb(uint8_t* data, uint8_t len) {
    return;
}

// VIA handles received HID messages first, and will route to
// raw_hid_receive_kb() for command IDs that are not handled here.
// This gives the keyboard code level the ability to handle the command
// specifically.
//
// raw_hid_send() is called at the end, with the same buffer, which was
// possibly modified with returned values.
void raw_hid_receive(uint8_t *data, uint8_t length) {
    uint8_t *command_id   = &(data[0]);
    uint8_t *command_data = &(data[1]);
    switch (*command_id) {
        case id_get_protocol_version: {
            command_data[0] = VIA_PROTOCOL_VERSION >> 8;
            command_data[1] = VIA_PROTOCOL_VERSION & 0xFF;
            break;
        }
        case id_get_keyboard_value: {
            switch (command_data[0]) {
                case id_uptime: {
                    uint32_t value  = 0xC0FFEE;//timer_read32();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }
                /*
                case id_layout_options: {
                    uint32_t value  = via_get_layout_options();
                    command_data[1] = (value >> 24) & 0xFF;
                    command_data[2] = (value >> 16) & 0xFF;
                    command_data[3] = (value >> 8) & 0xFF;
                    command_data[4] = value & 0xFF;
                    break;
                }
                */
                case id_switch_matrix_state: {
                    if ((via_setting.col_count / 8 + 1) * via_setting.row_count <= 28) {
                        uint8_t i = 1;
                        for (uint8_t row = 0; row < via_setting.row_count; row++) {
                            matrix_row_t value = 8;//matrix_get_row(row);
                            if (via_setting.col_count > 24){
                              command_data[i++] = (value >> 24) & 0xFF;
                            }
                            if (via_setting.col_count > 16){
                              command_data[i++] = (value >> 16) & 0xFF;
                            }
                            if (via_setting.col_count > 8){
                              command_data[i++] = (value >> 8) & 0xFF;
                            }
                            command_data[i++] = value & 0xFF;
                        }
                    }
                    break;
                }
                default: {
                    raw_hid_receive_kb(data, length);
                    break;
                }
            }
            break;
        }
        case id_set_keyboard_value: {
            switch (command_data[0]) {
                /*
                case id_layout_options: {
                    uint32_t value = ((uint32_t)command_data[1] << 24) | ((uint32_t)command_data[2] << 16) | ((uint32_t)command_data[3] << 8) | (uint32_t)command_data[4];
                    via_set_layout_options(value);
                    break;
                }
                */
                default: {
                    raw_hid_receive_kb(data, length);
                    break;
                }
            }
            break;
        }
        case id_dynamic_keymap_get_keycode: {
            uint16_t keycode = dynamic_keymap_get_keycode(command_data[0], command_data[1], command_data[2]);
            command_data[3]  = keycode >> 8;
            command_data[4]  = keycode & 0xFF;
            break;
        }
        case id_dynamic_keymap_set_keycode: {
            dynamic_keymap_set_keycode(command_data[0], command_data[1], command_data[2], (command_data[3] << 8) | command_data[4]);
            keymap_updated = true;
            keymap_saved = false;
            break;
        }
        case id_dynamic_keymap_reset: {
            //dynamic_keymap_reset();
            break;
        }
        case id_dynamic_keymap_macro_get_count: {
            command_data[0] = 0; //dynamic_keymap_macro_get_count();
            break;
        }
        case id_dynamic_keymap_macro_get_buffer_size: {
            uint16_t size   = 0x0;//dynamic_keymap_macro_get_buffer_size();
            command_data[0] = size >> 8;
            command_data[1] = size & 0xFF;
            break;
        }
        case id_dynamic_keymap_macro_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            //dynamic_keymap_macro_get_buffer(offset, size, &command_data[3]);
            break;
        }
        case id_dynamic_keymap_macro_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            //dynamic_keymap_macro_set_buffer(offset, size, &command_data[3]);
            break;
        }
        case id_dynamic_keymap_macro_reset: {
            //dynamic_keymap_macro_reset();
            break;
        }
        case id_dynamic_keymap_get_layer_count: {
            command_data[0] = dynamic_keymap_get_layer_count();
            break;
        }
        case id_dynamic_keymap_get_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            dynamic_keymap_get_buffer(offset, size, &command_data[3]);
            if(! keymap_saved ) {
                save_keymap();
                keymap_saved = true;
            }
            break;
        }
        case id_dynamic_keymap_set_buffer: {
            uint16_t offset = (command_data[0] << 8) | command_data[1];
            uint16_t size   = command_data[2];  // size <= 28
            dynamic_keymap_set_buffer(offset, size, &command_data[3]);
            keymap_updated = true;
            keymap_saved = false;
            break;
        }
        default: {
            // The command ID is not known
            // Return the unhandled state
            *command_id = id_unhandled;
            break;
        }
    }

    // Return the same buffer, optionally with values changed
    // (i.e. returning state to the host, or the unhandled state).
    if(length > REPORT_RAW_MAX_LEN) {
        length = REPORT_RAW_MAX_LEN;
    }
    if(data[0] != id_unhandled) {
        raw_hid_send(data, length);
    }
}
