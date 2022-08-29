#include <mrubyc.h>

#include "hardware/pio.h"
#include "hardware/dma.h"
#include "ws2812.pio.h"
#include "uart_tx.pio.h"
#include "sounder.pio.h"

#define PIO_SOUNDER_INST_HEAD ( uart_tx_program.length + ws2812_program.length )
#define SONG_MAX_LEN 128
#define SOUNDER_BASE_HZ (100000UL)

static PIO pio = pio1;
static uint sm = 2;

static uint32_t song_data[SONG_MAX_LEN];
static int sounder_dma_channel = -1;

static void
init_dma_sounder(void)
{
  if(sounder_dma_channel < 0) {
    sounder_dma_channel = dma_claim_unused_channel(true);
  }
  
  dma_channel_config c = dma_channel_get_default_config(sounder_dma_channel);
  channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
  channel_config_set_read_increment(&c, true);
  channel_config_set_write_increment(&c, false);
  channel_config_set_dreq(&c, ( pio==pio0 ? DREQ_PIO0_TX0 : DREQ_PIO1_TX0 ) + sm);
  channel_config_set_irq_quiet(&c, true);

  dma_channel_configure(
      sounder_dma_channel, // Channel to be configured
      &c,               // The configuration we just created
      pio->txf+sm,      // The initial write address
      song_data,        // The initial read address
      SONG_MAX_LEN,     // Number of transfers; in this case each is 4 byte.
      false             // Start immediately.
  );
}

void
c_sounder_init(mrb_vm *vm, mrb_value *v, int argc)
{
  if(pio_can_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD)) {
    pio_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD);
  }
  
  sounder_program_init(pio, sm, PIO_SOUNDER_INST_HEAD, GET_INT_ARG(1));
  
  init_dma_sounder();
}

void
c_sounder_set_tones(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_array *rb_ary = GET_ARY_ARG(1).array;
  uint8_t sounder_count = ( rb_ary->n_stored < SONG_MAX_LEN ) ? (rb_ary->n_stored) : (SONG_MAX_LEN);

  for (uint8_t i=0; i<sounder_count; i++)
  {
    mrbc_array *item = rb_ary->data[i].array;
    uint16_t frequency = mrbc_integer(item->data[0]);
    uint16_t duration = mrbc_integer(item->data[1]);
    
    if (frequency)
    {
      song_data[i] = ( (SOUNDER_BASE_HZ/frequency) & 0x0FFF ) | ( (duration*frequency<<2) & 0xFFFFF000UL );
    } else {
      song_data[i] = (duration<<12);
    }
  }

  for (uint8_t i=sounder_count; i<SONG_MAX_LEN; i++)
  {
    song_data[i] = 0;
  }
}

void
c_sounder_start(mrb_vm *vm, mrb_value *v, int argc)
{
  dma_channel_abort(sounder_dma_channel);
  pio_sm_drain_tx_fifo(pio, sm);
  dma_channel_set_read_addr(sounder_dma_channel, song_data, true);
}
