#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "usb_descriptors.h"
#include "gamepad.h"

#define MAX_ADC_COUNT  4

#define AXIS_INDEX_X   0
#define AXIS_INDEX_Y   1
#define AXIS_INDEX_Z   2
#define AXIS_INDEX_RZ  3
#define AXIS_INDEX_RX  4
#define AXIS_INDEX_RY  5
static int8_t axes[MAX_ADC_COUNT] = {-1,-1,-1,-1};
static uint8_t adc_offset[MAX_ADC_COUNT] = {0,0,0,0};

static uint32_t buttons = 0;
static uint8_t hat = 0;

void
c_set_values(mrb_vm *vm, mrb_value *v, int argc)
{
  buttons = GET_INT_ARG(1);
  hat = GET_INT_ARG(2);
  SET_NIL_RETURN();
}

void
c_reset_axes(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < MAX_ADC_COUNT; i++) {
    axes[i] = -1;
    adc_offset[i] = 0;
  }
  SET_NIL_RETURN();
}

void
c_init_axis_offset(mrb_vm *vm, mrb_value *v, int argc)
{
  char *axis = GET_STRING_ARG(1);
  int8_t adc_ch = GET_INT_ARG(2);
  adc_gpio_init(adc_ch);
  adc_select_input(adc_ch);
  uint16_t offset_sum = 0;
  for (int i = 0; i < 3; i++) {
    sleep_ms(20);
    offset_sum += adc_read();
  }
  adc_offset[adc_ch] = (offset_sum / 3) >> 4;
  if (strcmp(axis, "x") == 0) {
    axes[adc_ch] = AXIS_INDEX_X;
  } else if (strcmp(axis, "y") == 0) {
    axes[adc_ch] = AXIS_INDEX_Y;
  } else if (strcmp(axis, "z") == 0) {
    axes[adc_ch] = AXIS_INDEX_Z;
  } else if (strcmp(axis, "rz") == 0) {
    axes[adc_ch] = AXIS_INDEX_RZ;
  } else if (strcmp(axis, "rx") == 0) {
    axes[adc_ch] = AXIS_INDEX_RX;
  } else if (strcmp(axis, "ry") == 0) {
    axes[adc_ch] = AXIS_INDEX_RY;
  } else {
    console_printf("Invalid analog stick: %s\n", axis);
    SET_FALSE_RETURN();
    return;
  }
  SET_TRUE_RETURN();
}

void
gamepad_report(void)
{
  hid_gamepad_report_t report = {
    .x = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0, .hat = 0, .buttons = buttons
  };
  /* analog axes */
  int16_t value;
  for (int ch = 0; ch < MAX_ADC_COUNT; ch++) {
    if (axes[ch] > -1) {
      adc_select_input(ch);
      value = (adc_read() >> 4)  - adc_offset[ch];
      if (value < -128) value = -128;
      if (127 < value) value = 127;
      switch (axes[ch]) {
        case AXIS_INDEX_X:  report.x  = (int8_t)value; break;
        case AXIS_INDEX_Y:  report.y  = (int8_t)value; break;
        case AXIS_INDEX_Z:  report.z  = (int8_t)value; break;
        case AXIS_INDEX_RZ: report.rz = (int8_t)value; break;
        case AXIS_INDEX_RX: report.rx = (int8_t)value; break;
        case AXIS_INDEX_RY: report.ry = (int8_t)value; break;
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
  tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
}
