#include "ws2812.h"

static PIO pio = pio1;
static uint sm = 0;

static inline void put_pixel(uint32_t pixel_grb) {
  pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
  return
    ((uint32_t) (r) << 8) |
    ((uint32_t) (g) << 16) |
    (uint32_t) (b);
}

void
c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint offset = pio_add_program(pio, &ws2812_program);
  bool is_rgbw;
  if (GET_ARG(2).tt == MRBC_TT_TRUE) {
    is_rgbw = true;
  } else {
    is_rgbw = false;
  }
  ws2812_program_init(pio, sm, offset, GET_INT_ARG(1), 800000, is_rgbw);
}

void
c_ws2812_resume(mrb_vm *vm, mrb_value *v, int argc)
{
  if (tcb_rgb == NULL) return;
  mrbc_resume_task(tcb_rgb);
}

void
c_ws2812_off(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < GET_INT_ARG(1); i++)
    put_pixel(0);
}

void
c_ws2812_show(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_array *array = GET_ARG(1).array;
  for (int i = 0; i < array->n_stored; i++)
    put_pixel((uint32_t)array->data[i].i);
}
