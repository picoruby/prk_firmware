#include "ws2812.h"

#include <stdlib.h>
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "hardware/clocks.h"

static PIO pio = pio1;
static uint sm = 0;

#define MAX_PIXEL_SIZE 100
#define MAX_BRIGHTNESS 100

static int32_t pixels[MAX_PIXEL_SIZE + 1];
static int32_t pixels_tmp[MAX_PIXEL_SIZE + 1];
static uint8_t brightness;

static inline uint32_t
rgb2grb(int32_t val) {
  return
    (uint32_t)((val & 0xff0000) >> 8) |
    (uint32_t)((val & 0x00ff00) << 8) |
    (uint32_t) (val & 0x0000ff);
}

static inline void
put_pixel(int32_t pixel_rgb) {
  pio_sm_put_blocking(pio, sm, rgb2grb(pixel_rgb) << 8u);
}

static inline void
set_at(uint8_t i, int32_t val)
{
  if (val < 0) return;
  pixels[i] = val;
}

void
c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint offset = pio_add_program(pio, &ws2812_program);
  bool is_rgbw;
  if (GET_ARG(3).tt == MRBC_TT_TRUE) {
    is_rgbw = true;
  } else {
    is_rgbw = false;
  }
  ws2812_program_init(pio, sm, offset, GET_INT_ARG(1), 800000, is_rgbw);
  int i;
  for (i = 0; i < GET_INT_ARG(2); i++) pixels[i] = 0;
  pixels[i] = -1; /* sentinel */
  pixels[MAX_PIXEL_SIZE] = -1;
  brightness = 0x20;
}

void
c_ws2812_set_pixel_at(mrb_vm *vm, mrb_value *v, int argc)
{
  set_at((uint8_t)GET_INT_ARG(1), GET_INT_ARG(2));
}

void
c_ws2812_fill(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < GET_INT_ARG(2); i++) {
    put_pixel(GET_INT_ARG(1));
  }
}

void
c_ws2812_rotate(mrb_vm *vm, mrb_value *v, int argc)
{
  int32_t first = pixels[0];
  int i = 1;
  while (pixels[i] >= 0) {
    pixels[i-1] = pixels[i];
    i++;
  }
  pixels[i-1] = first;
}

void
c_ws2812_show(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = 0;
  while (pixels[i] >= 0) {
    put_pixel((uint32_t)pixels[i]);
    i++;
  }
}

void
c_ws2812_save(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = 0;
  while (pixels[i] >= 0) {
    pixels_tmp[i] = pixels[i];
    i++;
  }
  pixels_tmp[i] = -1;
}

void
c_ws2812_restore(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = 0;
  while (pixels_tmp[i] >= 0) {
    pixels[i] = pixels_tmp[i];
    i++;
  }
  pixels[i] = -1;
}

void
c_ws2812_rand_show(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < GET_INT_ARG(3); i++) {
    if (rand() % 16 > GET_INT_ARG(2)) {
      put_pixel(GET_INT_ARG(1));
    } else {
      put_pixel(0);
    }
    i++;
  }
}
