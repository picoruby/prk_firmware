#include <stdlib.h>
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "uart_tx.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

#include "../include/ws2812.h"

#define PIO_WS2812_INST_HEAD uart_tx_program.length

static PIO pio = pio1;
static uint sm = 0;

void
RGB_dma_channel_set_read_addr(uint32_t channel, const volatile void *read_addr, bool trigger)
{
  dma_channel_set_read_addr((uint)channel, read_addr, trigger);
}

uint32_t
RGB_init_dma_ws2812(int channel, uint32_t leds_count, const volatile void *read_addr)
{
  if(channel < 0) {
    channel = dma_claim_unused_channel(true);
  }
  dma_channel_config c = dma_channel_get_default_config((uint)channel);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  channel_config_set_read_increment(&c, true);
  channel_config_set_write_increment(&c, false);
  channel_config_set_dreq(&c, (pio == pio0 ? DREQ_PIO0_TX0 : DREQ_PIO1_TX0) + sm);
  channel_config_set_irq_quiet(&c, true);

  dma_channel_configure(
    (uint)channel,    // Channel to be configured
    &c,               // The configuration we just created
    pio->txf+sm,      // The initial write address
    read_addr,        // The initial read address
    (uint)leds_count, // Number of transfers; in this case each is 4 byte.
    false             // Not start
  );

  return channel;
}

void
RGB_init_pio(uint32_t pin, bool is_rgbw)
{
  if (pio_can_add_program_at_offset(pio, &ws2812_program, PIO_WS2812_INST_HEAD)) {
    pio_add_program_at_offset(pio, &ws2812_program, PIO_WS2812_INST_HEAD);
    ws2812_program_init(pio, sm, PIO_WS2812_INST_HEAD, (uint)pin, is_rgbw);
  }
}

