#include "uart.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

void
c_uart_tx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uart_init(UART_ID, 2400);
  gpio_set_function(GET_INT_ARG(1), GPIO_FUNC_UART); // Set the RX pin
  uart_set_baudrate(UART_ID, BAUD_RATE);
  uart_set_hw_flow(UART_ID, false, false); // Turn UART flow control off
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Set data format
  uart_set_fifo_enabled(UART_ID, true); // Turn on FIFO
}

void
c_uart_putc_raw(mrb_vm *vm, mrb_value *v, int argc)
{
 uart_putc_raw(UART_ID, GET_INT_ARG(1));
}

/*
 * It seems TX pin can't be inverted to RX on Raspi Pico.
 * That's why using PIO instead.
 */
static PIO pio = pio0;
static uint sm = 0;
void
c_uart_rx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint offset = pio_add_program(pio, &uart_rx_program);
  uart_rx_program_init(pio, sm, offset, GET_INT_ARG(1), BAUD_RATE);
}

void
c_uart_getc(mrb_vm *vm, mrb_value *v, int argc)
{
  if (pio_sm_is_rx_fifo_empty(pio, sm)) {
    SET_NIL_RETURN();
    return;
  }
  // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
  io_rw_8 *rxfifo_shift = (io_rw_8*)&pio->rxf[sm] + 3;
  while (pio_sm_is_rx_fifo_empty(pio, sm))
    tight_loop_contents();
  SET_INT_RETURN((int)*rxfifo_shift);
}
