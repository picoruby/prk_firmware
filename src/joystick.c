#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#include "../include/usb_descriptors.h"
#include "../include/joystick.h"
#include "picoruby-prk-joystick/include/prk-joystick.h"

static hid_gamepad_report_t zero_report;

/*
 * TODO: Reconsuruct these functions with picoruby-adc gem
 */

void
Joystick_adc_gpio_init(uint32_t gpio)
{
  static bool init = false;
  if (!init) {
    adc_init();
    init = true;
  }
  adc_gpio_init((uint)gpio);
}

void
Joystick_adc_select_input(uint32_t input)
{
  adc_select_input((uint)input);
}

uint16_t
Joystick_adc_read(void)
{
  return adc_read();
}

void
Joystick_reset_zero_report(void)
{
  memset(&zero_report, 0, sizeof(zero_report));
}

void
joystick_report_hid(uint32_t buttons, uint8_t hat)
{
  static bool zero_reported = false;
  hid_gamepad_report_t report;
  memset(&report, 0, sizeof(report));
  /* buttons */
  report.buttons = buttons;
  /* analog axes */
  int16_t value;
  for (uint ch = 0; ch < MAX_ADC_COUNT; ch++) {
    if (Joystick_axes[ch] > -1) {
      adc_select_input(ch);
      value = (int16_t)(((adc_read() >> 4) - Joystick_adc_offset[ch]) * Joystick_sensitivity[ch]);
      if ((value < Joystick_drift_suppression_minus) || (Joystick_drift_suppression < value)) {
        if (value < -128) {
          value = -128;
        } else if (127 < value) {
          value = 127;
        }
        switch (Joystick_axes[ch]) {
          case AXIS_INDEX_X:  report.x  = (int8_t)value; break;
          case AXIS_INDEX_Y:  report.y  = (int8_t)value; break;
          case AXIS_INDEX_Z:  report.z  = (int8_t)value; break;
          case AXIS_INDEX_RZ: report.rz = (int8_t)value; break;
          case AXIS_INDEX_RX: report.rx = (int8_t)value; break;
          case AXIS_INDEX_RY: report.ry = (int8_t)value; break;
        }
      }
    }
  }
  /*
   * uint8_t
   * hat: 0b00001111
   *               ^right
   *              ^up
   *             ^down
   *            ^left
   * GAMEPAD_HAT_CENTERED    0
   * GAMEPAD_HAT_UP          1
   * GAMEPAD_HAT_UP_RIGHT    2
   * GAMEPAD_HAT_RIGHT       3
   * GAMEPAD_HAT_DOWN_RIGHT  4
   * GAMEPAD_HAT_DOWN        5
   * GAMEPAD_HAT_DOWN_LEFT   6
   * GAMEPAD_HAT_LEFT        7
   * GAMEPAD_HAT_UP_LEFT     8
   */
  if (hat <= 0b0100) {
    switch (hat) {
      case 0b0001: report.hat = GAMEPAD_HAT_RIGHT;      break;
      case 0b0010: report.hat = GAMEPAD_HAT_UP;         break;
      case 0b0011: report.hat = GAMEPAD_HAT_UP_RIGHT;   break;
      case 0b0100: report.hat = GAMEPAD_HAT_DOWN;       break;
    }
  } else {
    switch (hat) {
      case 0b0101: report.hat = GAMEPAD_HAT_DOWN_RIGHT; break;
      case 0b1000: report.hat = GAMEPAD_HAT_LEFT;       break;
      case 0b1010: report.hat = GAMEPAD_HAT_UP_LEFT;    break;
      case 0b1100: report.hat = GAMEPAD_HAT_DOWN_LEFT;  break;
    }
  }
  if (memcmp(&report, &zero_report, sizeof(report))) {
    tud_hid_n_report(1, REPORT_ID_GAMEPAD, &report, sizeof(report));
    zero_reported = false;
  } else if (!zero_reported) {
    tud_hid_n_report(1, REPORT_ID_GAMEPAD, &report, sizeof(report));
    zero_reported = true;
  }
}
