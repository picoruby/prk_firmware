#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "usb_descriptors.h"
#include "gamepad.h"


#define ADC_INDEX_X   0
#define ADC_INDEX_Y   1
#define ADC_INDEX_Z   2
#define ADC_INDEX_RZ  3
#define ADC_COUNT     4
static int8_t adc_table[ADC_COUNT];

void
c_init_adc_table(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < ADC_COUNT; i++) {
    adc_table[i] = -1;
  }
}

void
c_init_joystick(mrb_vm *vm, mrb_value *v, int argc)
{
  char *axis = GET_STRING_ARG(1);
  int pin = GET_INT_ARG(2);
  int8_t adc_num;
  switch (pin) {
    case (26): adc_num = 0; break;
    case (27): adc_num = 1; break;
    case (28): adc_num = 2; break;
    case (29): adc_num = 3; break;
    default:
      console_printf("Invaid joystick pin: %d\n", pin);
      return;
  }
  adc_gpio_init(pin);
  switch (axis[0]) {
    case 'x': adc_table[ADC_INDEX_X]  = adc_num; break;
    case 'y': adc_table[ADC_INDEX_Y]  = adc_num; break;
    case 'z': adc_table[ADC_INDEX_Z]  = adc_num; break;
    case 'r': adc_table[ADC_INDEX_RZ] = adc_num; break;
    default:
      console_printf("Invalid joystick axis: %s\n", axis);
  }
}

void
c_report_gamepad(mrb_vm *vm, mrb_value *v, int argc)
{
  hid_gamepad_report_t report = {
    .x   = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0,
    .hat = 0, .buttons = GET_INT_ARG(1)
  };
  for (int i = 0; i < ADC_COUNT; i++) {
    if (adc_table[i] > -1) {
      adc_select_input(adc_table[i]);
      switch (i) {
        case ADC_INDEX_X:  report.x  = (adc_read() >> 4) - 127; break;
        case ADC_INDEX_Y:  report.y  = (adc_read() >> 4) - 127; break;
        case ADC_INDEX_Z:  report.z  = (adc_read() >> 4) - 127; break;
        case ADC_INDEX_RZ: report.rz = (adc_read() >> 4) - 127; break;
      }
    }
  }
  tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
  console_printf("\e[0EX:%04d Y:%04d Z:%04d RZ:%04d", report.x, report.y, report.z, report.rz);
}


