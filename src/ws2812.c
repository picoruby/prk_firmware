#include "../include/ws2812.h"

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

static int32_t pixels[MAX_PIXEL_SIZE + 1] = {};
static int32_t dma_ws2812_grb_pixels[MAX_PIXEL_SIZE];
static int dma_ws2812_channel = -1;
static uint8_t dma_ws2812_last_append_index = 0;


static inline uint32_t
rgb2grb(int32_t val) {
  return
    (uint32_t)((val & 0xff0000) >> 8) |
    (uint32_t)((val & 0x00ff00) << 8) |
    (uint32_t) (val & 0x0000ff);
}

static void
put_pixel(int32_t pixel_rgb) {
  dma_ws2812_grb_pixels[dma_ws2812_last_append_index++] = rgb2grb(pixel_rgb) << 8u;
}

static inline void 
show_pixels(void) {
  dma_channel_set_read_addr(dma_ws2812_channel, dma_ws2812_grb_pixels, true);
  dma_ws2812_last_append_index = 0;
}

static void
init_dma_ws2812(uint16_t leds_count)
{
  if(dma_ws2812_channel < 0) {
    dma_ws2812_channel = dma_claim_unused_channel(true);
  }
  
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
    leds_count,          // Number of transfers; in this case each is 4 byte.
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

  init_dma_ws2812(GET_INT_ARG(2));
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
  show_pixels();
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
  show_pixels();
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
  show_pixels();
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
  show_pixels();
}


/******************************************************
 *
 * Functions and variables for RGB MATRIX
 *
 ******************************************************/

static uint8_t matrix_coordinate[MAX_PIXEL_SIZE][2];
static uint32_t pixel_distance[MAX_PIXEL_SIZE];
static uint8_t circle_center[2] = { 112, 32 };

void
c_ws2812_add_matrix_pixel_at(mrb_vm *vm, mrb_value *v, int argc)
{
  uint16_t i = GET_INT_ARG(1);
  matrix_coordinate[i][0] = (uint8_t)GET_INT_ARG(2);
  matrix_coordinate[i][1] = (uint8_t)GET_INT_ARG(3);
}

void
c_ws2812_circle_set_center(mrb_vm *vm, mrb_value *v, int argc)
{
  circle_center[0] = GET_INT_ARG(1);
  circle_center[1] = GET_INT_ARG(2);
}

void
c_ws2812_init_pixel_distance(mrb_vm *vm, mrb_value *v, int argc)
{
  int32_t x;
  int32_t y;
  for (int i=0; i < GET_INT_ARG(1); i++) {
    x = matrix_coordinate[i][0] - circle_center[0];
    y = (matrix_coordinate[i][1] - circle_center[1]) * 3;
    pixel_distance[i] = x * x + y * y;
  }
}

static inline uint8_t get_distance_group(uint32_t d) {
  if(d<400) {
    // 20
    return 0;
  } else if(d<1600) {
    // 40
    return 1;
  } else if(d<3600) {
    // 60
    return 2;
  } else if(d<6400) {
    // 80
    return 3;
  } else if(d<10000) {
    // 100
    return 4; 
  } else if(d<14400) {
    // 120
    return 5;
  } else if(d<19600) {
    // 140
    return 6;
  } else if(d<25600) {
    // 160
    return 7;
  } else if(d<32800) {
    // 180
    return 8;
  } else if(d<40000) {
    // 200
    return 9;
  } else if(d<48400) {
    // 220
    return 10;
  } else if(d<57600) {
    // 240
    return 11;
  } else {
    return 12;
  }
}

#define RGB(r,g,b) ( r<<16 | g<<8 | b )

void
c_ws2812_circle(mrb_vm *vm, mrb_value *v, int argc)
{
  int count = GET_INT_ARG(1);
  int value = GET_INT_ARG(2);
  int circle_diameter = GET_INT_ARG(3);

  uint8_t lv = value*2;

  for(uint16_t i=0; i<count; i++) {
    uint8_t b = get_distance_group(pixel_distance[i]);
    
    b = (b+circle_diameter) %12;
    switch(b) {
      case 0:
        put_pixel( RGB(lv*4, 0 ,0 ) );
        break;
      case 1:
        put_pixel( RGB(lv*3, lv, 0) );
        break;
      case 2:
        put_pixel( RGB(lv*2, lv*2, 0) );
        break;
      case 3:
        put_pixel( RGB(lv, lv*3, 0) );
        break;
      case 4:
        put_pixel( RGB(0, lv*4, 0) );
        break;
      case 5:
        put_pixel( RGB(0, lv*3, lv) );
        break;
      case 6:
        put_pixel( RGB(0, lv*2, lv*2) );
        break;
      case 7:
        put_pixel( RGB(0, lv, lv*3) );
        break;
      case 8:
        put_pixel( RGB(0, 0, lv*4) );
        break;
      case 9:
        put_pixel( RGB(lv, 0, lv*3) );
        break;
      case 10:
        put_pixel( RGB(lv*2, 0, lv*2) );
        break;
      case 11:
        put_pixel( RGB(lv*3, 0, lv) );
        break;
      default:
        put_pixel( RGB(lv, lv, lv) );
        break;
    }
  }
  show_pixels();
}

