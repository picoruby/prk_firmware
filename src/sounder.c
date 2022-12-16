#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"
#include "sounder.pio.h"

#include "../include/sounder.h"
#include "picoruby-prk-sounder/include/prk-sounder.h"

#define PIO_SOUNDER_INST_HEAD (ws2812_program.length)
#define SOUNDER_PIO_FINISH_IRQ_ID PIO1_IRQ_1

static PIO pio = pio1;
static uint sm = 2;

static void
sounder_resume_handler(void)
{
  pio_interrupt_clear(pio, 1);
  irq_clear(SOUNDER_PIO_FINISH_IRQ_ID);

  if(Sounder_song_data_index == Sounder_song_data_len) { return; }

  float hz = Sounder_song_data[Sounder_song_data_index];
  uint16_t len = Sounder_song_len[Sounder_song_data_index];

  if(len == 0) { return; }

  if(hz == 0) {
    pio_sm_set_clkdiv(pio, sm, (float)clock_get_hz(clk_sys) / (SOUNDER_DEFAULT_HZ * SOUNDER_SM_CYCLES));
    pio_sm_put_blocking(pio, sm, len);
  } else {
    pio_sm_set_clkdiv(pio, sm, (float)clock_get_hz(clk_sys) / hz);
    pio_sm_put_blocking(pio, sm, (1<<31) | len);
  }
  Sounder_song_data_index++;
}

void
Sounder_init(uint32_t pin)
{
  if(pio_can_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD)) {
    pio_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD);
  }
  sounder_program_init(pio, sm, PIO_SOUNDER_INST_HEAD, (uint)pin);
  pio_set_irq1_source_enabled(pio, pis_interrupt1, true);
  irq_set_exclusive_handler(SOUNDER_PIO_FINISH_IRQ_ID, sounder_resume_handler);
  irq_set_priority(SOUNDER_PIO_FINISH_IRQ_ID, PICO_DEFAULT_IRQ_PRIORITY);
  irq_set_enabled(SOUNDER_PIO_FINISH_IRQ_ID, true);
}

void
Sounder_replay(void)
{
  pio_sm_drain_tx_fifo(pio, sm);
  Sounder_song_data_index = 0;
  sounder_resume_handler();
}
