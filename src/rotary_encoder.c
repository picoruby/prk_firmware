#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "rotary_encoder.h"

static int8_t encoder_count = 0;
static int8_t pins_a[] = {-1,-1,-1,-1,-1};
static int8_t pins_b[] = {-1,-1,-1,-1,-1};
volatile static int8_t status[] = {0,0,0,0,0};
volatile static int8_t rotation_count[] = {0,0,0,0,0};
#define MAX_ENCODER_COUNT 5

void
RotaryEncoder_reset(void)
{
  encoder_count = 0;
  for (int i = 0; i < MAX_ENCODER_COUNT; i++) {
    pins_a[i] = -1;
    pins_b[i] = -1;
    status[i] = 0;
    rotation_count[i] = 0;
  }
}

/*
 * status[i]:
 * 0b111111
 *       ^^ current status
 *     ^^   previout status
 *   ^^     possible rotate direction
 *
 *        ccw <-|-> cw
 * pin_a: 1 1 0 0 1 1 0
 * pin_b: 0 1 1 0 0 1 1
 *             (^ start position)
 * (A-1) When the previous rotation is zero (it means just started rotating), and
 * (A-2) if the status shifts right, rotation is possibly clockwise (CW).
 *       if the status shifts left, rotation is possibly counterclockwise (CCW).
 * (B-1) When the status comes back to the start position, and
 * (B-2) if the possible rotation is cw and the previous status is `01`, should be CW.
 *       if the possible rotation is ccw and the previous status is `10`, should be CCW.
 */
void
encoder_callback(uint gpio, uint32_t _events) /* We ignore the event itself */
{
  for (int i = 0; i < MAX_ENCODER_COUNT; i++) {
    if (pins_a[i] < 0) return;
    if (gpio == pins_a[i] || gpio == pins_b[i]) {
      uint8_t current = (!gpio_get(pins_a[i]) << 1) + !gpio_get(pins_b[i]);
      uint8_t prev = status[i] & 0b00000011;
      uint8_t rotation = (status[i] & 0b00110000) >> 4;
      /* Processes iff the status changed in order to debounce */
      if (current != prev) {
        /* (A-1) */
        if (rotation == 0b00) {
          /* (A-2) */
          if (current == 0b01 || current == 0b10) {
            rotation = current; /* Sets possible ratation */
          }
        /* (B-1) */
        } else if (current == 0b00) {
          /* (B-2) */
          if (rotation == 0b10 && prev == 0b01) {
            rotation_count[i]++; /* CW */
          } else if (rotation == 0b01 && prev == 0b10) {
            rotation_count[i]--; /* CCW */
          }
          rotation = 0b00;
        }
        status[i] = (rotation << 4) + (prev << 2) + current;
      }
    }
  }
}

void
c_consume_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  int number = GET_INT_ARG(1);
  if (rotation_count[number] > 0) {
    SET_INT_RETURN(1);
    rotation_count[number]--;
  } else if (rotation_count[number] < 0) {
    SET_INT_RETURN(-1);
    rotation_count[number]++;
  } else {
    SET_INT_RETURN(0);
  }
}

void
c_init_encoder(mrb_vm *vm, mrb_value *v, int argc)
{
  uint32_t events = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;
  if (encoder_count == 0) {
     gpio_set_irq_enabled_with_callback(0, events, false, &encoder_callback);
  } else if (encoder_count > 4) {
    console_printf("RotaryEncoderLimitationError.\n");
    return;
  }
  uint8_t pin_a = GET_INT_ARG(1);
  uint8_t pin_b = GET_INT_ARG(2);
  gpio_init(pin_a);
  gpio_set_dir(pin_a, GPIO_IN);
  gpio_pull_up(pin_a);
  gpio_set_irq_enabled(pin_a, events, true);
  pins_a[encoder_count] = pin_a;
  gpio_init(pin_b);
  gpio_set_dir(pin_b, GPIO_IN);
  gpio_pull_up(pin_b);
  gpio_set_irq_enabled(pin_b, events, true);
  pins_b[encoder_count] = pin_b;
  SET_INT_RETURN(encoder_count);
  encoder_count++;
}
