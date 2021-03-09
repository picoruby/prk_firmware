#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "bsp/board.h"

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
 //   TUD_HID_REPORT_DESC_GENERIC_INOUT(1)
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

void hid_task(void) {
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

    /*------------- Keyboard -------------*/
    if (tud_hid_ready()) {
        // use to avoid send multiple consecutive zero report for keyboard
        static bool has_key = false;

        static bool toggle = false;
        if (toggle = !toggle) {
            uint8_t keycode[6] = {0};
            keycode[0] = HID_KEY_A;
            keycode[1] = HID_KEY_B;
            keycode[2] = HID_KEY_C;

            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);

            has_key = true;
        } else {
            // send empty key report if previously has key pressed
            if (has_key) tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
            has_key = false;
        }
    }
}

int main() {
  board_init();
  tusb_init();

  while(1) {
    tud_task();
    hid_task();
  }
  return 0;
}
