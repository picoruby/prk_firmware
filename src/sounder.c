#include <mrubyc.h>

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "ws2812.pio.h"
#include "uart_tx.pio.h"
#include "sounder.pio.h"

#define PIO_SOUNDER_INST_HEAD ( uart_tx_program.length + ws2812_program.length )
#define SOUNDER_PIO_FINISH_IRQ_ID PIO1_IRQ_1
#define SOUNDER_DEFAULT_HZ 2000
#define SOUNDER_SM_CYCLES 128
#define SONG_MAX_LEN 128

static PIO pio = pio1;
static uint sm = 2;

static float song_data[SONG_MAX_LEN] = {0};
static uint16_t song_len[SONG_MAX_LEN] = {0};
static uint8_t  song_data_index = 0;
static uint8_t  song_data_len = 0;

static void
sounder_resume_handler(void) {
  pio_interrupt_clear(pio, 1);
  irq_clear(SOUNDER_PIO_FINISH_IRQ_ID);

  if( song_data_index==song_data_len ) { return; }

  float hz = song_data[song_data_index];
  uint16_t len = song_len[song_data_index];
  
  if( len==0 ) { return; }

  if(hz==0) {
    pio_sm_set_clkdiv(pio, sm, (float)clock_get_hz(clk_sys) / (SOUNDER_DEFAULT_HZ*SOUNDER_SM_CYCLES));
    pio_sm_put_blocking(pio, sm, len);
  } else {
    pio_sm_set_clkdiv(pio, sm, (float)clock_get_hz(clk_sys) / hz);
    pio_sm_put_blocking(pio, sm, (1<<31) | len);
  }
  song_data_index++;
}

void
c_sounder_init(mrb_vm *vm, mrb_value *v, int argc)
{
  if(pio_can_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD)) {
    pio_add_program_at_offset(pio, &sounder_program, PIO_SOUNDER_INST_HEAD);
  }
  
  sounder_program_init(pio, sm, PIO_SOUNDER_INST_HEAD, GET_INT_ARG(1));
  pio_set_irq1_source_enabled(pio, pis_interrupt1, true);
  irq_set_exclusive_handler(SOUNDER_PIO_FINISH_IRQ_ID, sounder_resume_handler);
  irq_set_priority(SOUNDER_PIO_FINISH_IRQ_ID, PICO_DEFAULT_IRQ_PRIORITY); 
  irq_set_enabled(SOUNDER_PIO_FINISH_IRQ_ID, true);
}

void
c_sounder_clear_song(mrb_vm *vm, mrb_value *v, int argc)
{
  song_data_len = 0;
  memset(song_len, 0, sizeof(song_len));
}

void
c_sounder_add_note(mrb_vm *vm, mrb_value *v, int argc)
{
  float pitch = GET_FLOAT_ARG(1);
  uint16_t duration = GET_INT_ARG(2);

  if (SONG_MAX_LEN == song_data_len+1) {
    console_printf("SONG_MAX_LEN overflowed\n");
    SET_FALSE_RETURN();
    return;
  }

  if (0 < pitch) {
    song_data[song_data_len] = pitch*SOUNDER_SM_CYCLES;
    song_len[song_data_len] = pitch*duration/1000;
  } else {
    song_data[song_data_len] = 0;
    song_len[song_data_len] = SOUNDER_DEFAULT_HZ*duration/1000;
  }
  
  song_data_len++;
  SET_TRUE_RETURN();
}

void
c_sounder_replay(mrb_vm *vm, mrb_value *v, int argc)
{
  pio_sm_drain_tx_fifo(pio, sm);
  song_data_index = 0;
  sounder_resume_handler();
}
