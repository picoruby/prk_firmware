#include <mrubyc.h>

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "ws2812.pio.h"
#include "uart_tx.pio.h"
#include "tone.pio.h"

#define PIO_TONE_INST_HEAD ( uart_tx_program.length + ws2812_program.length )
#define TONE_MAX_LEN 64
#define TONE_BASE_HZ (100000UL)

static PIO pio = pio1;
static uint sm = 2;

static uint32_t tone_data[TONE_MAX_LEN];
static int tone_dma_channel = -1;

static void
init_dma_tone(void)
{
  if(tone_dma_channel < 0) {
    tone_dma_channel = 2; //dma_claim_unused_channel(true);
  }
  
  dma_channel_config c = dma_channel_get_default_config(tone_dma_channel);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  channel_config_set_read_increment(&c, true);
  channel_config_set_write_increment(&c, false);
  channel_config_set_dreq(&c, ( pio==pio0 ? DREQ_PIO0_TX0 : DREQ_PIO1_TX0 ) + sm);
  channel_config_set_irq_quiet(&c, true);

  dma_channel_configure(
      tone_dma_channel, // Channel to be configured
      &c,               // The configuration we just created
      pio->txf+sm,      // The initial write address
      tone_data,        // The initial read address
      TONE_MAX_LEN,     // Number of transfers; in this case each is 4 byte.
      false             // Start immediately.
  );
}

void
c_tone_init(mrb_vm *vm, mrb_value *v, int argc)
{
  if(pio_can_add_program_at_offset(pio, &tone_program, PIO_TONE_INST_HEAD)) {
    pio_add_program_at_offset(pio, &tone_program, PIO_TONE_INST_HEAD);
  }
  
  tone_program_init(pio, sm, PIO_TONE_INST_HEAD, GET_INT_ARG(1));
  
  init_dma_tone();
}

void
c_tone_set_tones(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_array *rb_ary = GET_ARY_ARG(1).array;
  uint8_t tone_count = ( rb_ary->n_stored < TONE_MAX_LEN ) ? (rb_ary->n_stored) : (TONE_MAX_LEN);

  for (uint8_t i=0; i<tone_count; i++)
  {
    mrbc_array *item = rb_ary->data[i].array;
    uint16_t tone = mrbc_integer(item->data[0]);
    uint16_t tone_ms = mrbc_integer(item->data[1]);
    
    if (tone==0)
    {
      tone = 1;
    }
    tone_data[i] = ( (TONE_BASE_HZ/tone) & 0x0FFF ) | ( (tone_ms*tone<<2) & 0xFFFFF000UL );
  }

  for (uint8_t i=tone_count; i<TONE_MAX_LEN; i++)
  {
    tone_data[i] = 0;
  }
}

void
c_tone_start(mrb_vm *vm, mrb_value *v, int argc)
{
  dma_channel_abort(tone_dma_channel);
  pio_sm_drain_tx_fifo(pio, sm);
  dma_channel_set_read_addr(tone_dma_channel, tone_data, true);
}