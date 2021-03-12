#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "bsp/board.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/structs/scb.h"
#include "hardware/sync.h"

/* mruby/c VM */
#include <mrubyc.h>

/* mini mruby compiler */
#include <mmrbc.h>

/* application */
#include "mrb_lib/led.c"
#include "mrb_lib/tud.c"
#include "mrb_lib/hid.c"

const uint8_t device_desc[] = {
	18, // bLength
	1, // bDescriptorType
	0x10,
	0x01, // bcdUSB
	0x00, // bDeviceClass
	0x00, // bDeviceSubClass
	0x00, // bDeviceProtocol
	CFG_TUD_ENDPOINT0_SIZE, // bMaxPacketSize0
	0xfe,
	0xca, // idVendor
	0xcd,
	0xab, // idProduct,
	0x00,
	0x00, // bcdDevice
	0x00, // iManufacturer
	0x01, // iProduct
	0x00, // iSerialNumber
	0x01, // bNumConfigurations
};

//const uint8_t conf_desc[] = {
//	9, // bLength
//	2, // bDescriptorType
//	9 + 9,
//	0, // wTotalLength
//	1, // bNumInterface
//	1, // bConfigurationValue
//	0, // iConfiguration
//	0x20, // bmAttributes
//	0x0F, // bMaxPower
//
//	// --- Interface ---
//	9, // bLength
//	4, // bDescriptorType
//	1, // bInterfaceNumber
//	0, // bAlternateSetting
//	0, // bNumEndpoints
//	0xFF, // bInterfaceClass
//	0xFF, // bInterfaceSubClass
//	0xFF, // bInterfaceProtocol
//	0, // iInterface
//};

const uint8_t hid_report_desc[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE))
};

const uint8_t conf_desc[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_HID_INOUT_DESC_LEN, 0, 0x0F),
    TUD_HID_INOUT_DESCRIPTOR(1, 0, 0, sizeof(hid_report_desc), 0x01, 0x81, 64, 0x0F)
};

const uint16_t string_desc_lang[] = { // Index: 0
	4 | (3 << 8), // bLength & bDescriptorType
	0
};
const uint16_t string_desc_product[] = { // Index: 1
	16 | (3 << 8),
	'R', 'a', 's', 'p', 'i', 'c', 'o'
};

uint8_t const *tud_descriptor_device_cb(void) {
	return device_desc;
}
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
	return conf_desc;
}
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
	uint16_t const *ret = NULL;
	switch(index) {
		case 0:
			ret = string_desc_lang;
			break;
		case 1:
			ret = string_desc_product;
			break;
		default:
			break;
	}
	return ret;
}


uint8_t const *tud_hid_descriptor_report_cb(void) {
    return hid_report_desc;
}
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    return 0;
}
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    return;
}

#define MEMORY_SIZE (1024*40)

static uint8_t memory_pool[MEMORY_SIZE];

static void c_led1_write(mrb_vm *vm, mrb_value *v, int argc) {
    int set_value = GET_INT_ARG(1);
    gpio_put(25,set_value);
}

static void c_tud_task(mrb_vm *vm, mrb_value *v, int argc) {
  tud_task();
}

static void c_hid_task(mrb_vm *vm, mrb_value *v, int argc) {
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms) return; // not enough time
    start_ms += interval_ms;

    uint32_t const btn = 1;

    // Remote wakeup
    if (tud_suspended() && btn) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }

    /*------------- Mouse -------------*/
    //if (tud_hid_ready()) {
    if (false) {
        if (btn) {
            int8_t const delta = 5;

            // no button, right + down, no scroll pan
            tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta, delta, 0, 0);

            // delay a bit before attempt to send keyboard report
            board_delay(10);
        }
    }

    uint8_t *keycode = GET_STRING_ARG(1);

    /*------------- Keyboard -------------*/
    if (tud_hid_ready()) {
        // use to avoid send multiple consecutive zero report for keyboard
        static bool has_key = false;
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);

        has_key = true;
        if (has_key && keycode[0] == 0) {
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
            has_key = false;
        }
    }
}

int main() {
  stdio_init_all();
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "led1_write",  c_led1_write);
  mrbc_define_method(0, mrbc_class_object, "tud_task",    c_tud_task);
  mrbc_define_method(0, mrbc_class_object, "hid_task",    c_hid_task);
  mrbc_create_task(led, 0);
  mrbc_create_task(tud, 0);
  mrbc_create_task(hid, 0);
  mrbc_run();
  return 0;
}
