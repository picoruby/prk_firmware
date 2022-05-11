#include "ws2812.h"

#include <stdlib.h>
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "uart_tx.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

static PIO pio = pio1;
static uint sm = 0;

#define PIO_WS2812_INST_HEAD uart_tx_program.length

#define MAX_PIXEL_SIZE 150

static int32_t pixels[MAX_PIXEL_SIZE + 1];
static int32_t dma_ws2812_grb_pixels[MAX_PIXEL_SIZE];
static int dma_ws2812_channel;
static uint8_t dma_ws2812_last_append_index = 0;
static uint32_t dma_ws2812_last_append_us = 0;

static inline uint32_t
rgb2grb(int32_t val) {
  return
    (uint32_t)((val & 0xff0000) >> 8) |
    (uint32_t)((val & 0x00ff00) << 8) |
    (uint32_t) (val & 0x0000ff);
}

static inline void
put_pixel(int32_t pixel_rgb) {
  uint32_t now = time_us_32();

  dma_ws2812_grb_pixels[dma_ws2812_last_append_index++] = rgb2grb(pixel_rgb) << 8u;
  
  // WS2812 signal chain is reset after 80 us
  if( now-dma_ws2812_last_append_us > 100 ) {
    pio_sm_put(pio, sm, dma_ws2812_grb_pixels[dma_ws2812_last_append_index-1]);
    dma_ws2812_last_append_index = 1;
  }

  if(dma_ws2812_last_append_index==2) {
    dma_channel_set_read_addr(dma_ws2812_channel, dma_ws2812_grb_pixels+1, true);
  }

  dma_ws2812_last_append_us = now;
}

static void
init_dma_ws2812(void)
{
  dma_ws2812_channel = dma_claim_unused_channel(true);

  dma_channel_config c = dma_channel_get_default_config(dma_ws2812_channel);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  channel_config_set_read_increment(&c, true);
  channel_config_set_write_increment(&c, false);
  channel_config_set_dreq(&c, ( pio==pio0 ? DREQ_PIO0_TX0 : DREQ_PIO1_TX0 ) + sm );
  channel_config_set_irq_quiet(&c, true);

  dma_channel_configure(
    dma_ws2812_channel,  // Channel to be configured
    &c,                  // The configuration we just created
    pio->txf+sm,         // The initial write address
    dma_ws2812_grb_pixels,      // The initial read address
    MAX_PIXEL_SIZE,      // Number of transfers; in this case each is 4 byte.
    false                // Not start
  );
}

void
c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc)
{
  bool is_rgbw;
  if (GET_ARG(3).tt == MRBC_TT_TRUE) {
    is_rgbw = true;
  } else {
    is_rgbw = false;
  }

  if(pio_can_add_program_at_offset(pio, &ws2812_program, PIO_WS2812_INST_HEAD)) {
    pio_add_program_at_offset(pio, &ws2812_program, PIO_WS2812_INST_HEAD);
    ws2812_program_init(pio, sm, PIO_WS2812_INST_HEAD, GET_INT_ARG(1), is_rgbw);
  }
  
  init_dma_ws2812();
}

void
c_ws2812_set_pixel_at(mrb_vm *vm, mrb_value *v, int argc)
{
  pixels[(uint8_t)GET_INT_ARG(1)] = GET_INT_ARG(2);
}

void
c_ws2812_fill(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 0; i < GET_INT_ARG(2); i++) {
    put_pixel(GET_INT_ARG(1));
  }
}

static int swirl_index = 0;
void
c_ws2812_rotate_swirl(mrb_vm *vm, mrb_value *v, int argc)
{
  volatile int at = swirl_index;
  int stop = GET_INT_ARG(1);
  for (int i = 0; i < stop; i++) {
    put_pixel(pixels[at]);
    at++;
    if (pixels[at] < 0) at = 0;
  }
  swirl_index++;
  if (pixels[swirl_index] < 0) {
    swirl_index = 0;
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

void
c_ws2812_reset_swirl_index(mrb_vm *vm, mrb_value *v, int argc)
{
  swirl_index = 0;
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
