#include "uart.h"

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "uart_tx.pio.h"
#include "pico/multicore.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

void
c_uart_rx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  // Set up UART with a basic baud rate.
  uart_init(UART_ID, 2400);
  gpio_set_function(GET_INT_ARG(1), GPIO_FUNC_UART); // Set the RX pin
  uart_set_baudrate(UART_ID, BAUD_RATE);
  uart_set_hw_flow(UART_ID, false, false); // Turn UART flow control off
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Set data format
  uart_set_fifo_enabled(UART_ID, true); // Turn on FIFO
}

void
c_uart_getc(mrb_vm *vm, mrb_value *v, int argc)
{
  if (uart_is_readable(UART_ID) == 0) {
    SET_NIL_RETURN();
    return;
  }
  SET_INT_RETURN(uart_getc(UART_ID));
}

/*
 * It seems RX pin can't be inverted to TX on Raspi Pico.
 * That's why using PIO instead.
 */
static PIO pio = pio0;
static uint sm = 0;
static uint offset;
void
c_uart_tx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  offset = pio_add_program(pio, &uart_tx_program);
  uart_tx_program_init(pio, sm, offset, GET_INT_ARG(1), BAUD_RATE);
}

void
c_uart_putc_raw(mrb_vm *vm, mrb_value *v, int argc)
{
  pio_sm_put_blocking(pio, sm, GET_INT_ARG(1));
}

/*
 * Bidirectional UART
 */
#define SAMPLING_INTERVAL 10
#define SAMPLING_COUNT     8
/*
 * (eg) 1-bit interval of 9600 bps is about 104 microseconds (usec)
 */
#define BIT_INTERVAL      (SAMPLING_INTERVAL * SAMPLING_COUNT)
#define NIL              255
#define LONG_STOP_BITS     9
#define SHORT_STOP_BITS    2
static int uart_pin;

//void
uint32_t
put8_get24_nonblocking(uint8_t data)
{
  uint8_t i;
  { /* TX */
    { /* Stop-bit */
      gpio_put(uart_pin, 1);
      sleep_us(BIT_INTERVAL * LONG_STOP_BITS);
    }
    { /* Start-bit */
      gpio_put(uart_pin, 0);
      sleep_us(BIT_INTERVAL);
    }
    { /* Send data */
      for (i = 0; i < DATA_BITS; i++) {
        gpio_put(uart_pin, (data >> i)&1);
        sleep_us(BIT_INTERVAL);
      }
    }
    { /* Stop-bit */
      gpio_put(uart_pin, 1);
      sleep_us(BIT_INTERVAL * SHORT_STOP_BITS);
    }
  }
  bool error = false;
  uint32_t data24;
  { /* RX */
    gpio_set_dir(uart_pin, GPIO_IN);
    gpio_pull_up(uart_pin);
    { /* Wait until finding a stop-bit */
      //while (gpio_get(uart_pin));
      /* You are on a possible start-bit */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin)) error = true;
      sleep_us(BIT_INTERVAL / 2);
    }
    { /* Receive a data */
      data24 = 0;
      for (i = 0; i < DATA_BITS * 3; i++) {
        sleep_us(BIT_INTERVAL / 2);
        if (gpio_get(uart_pin)) {
          data24 &= (1 << i); // Big endian
        }
        sleep_us(BIT_INTERVAL / 2);
      }
    }
    { /* Check the stop-bit */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin) == 0) data24 = NIL; // Something's wrong
      sleep_us(BIT_INTERVAL / 2);
    }
  }
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 0); // Idling
  if (error) return NIL;
  return data24;
}

uint8_t
put24_get8_blocking(uint8_t put_data)
{
  uint8_t data, i;
  for (;;) {
    { /* RX */
      { /* Wait until stop-bit appears */
        if (gpio_get(uart_pin)) continue;
        while (gpio_get(uart_pin) == 0);
        i = 0;
        while (gpio_get(uart_pin)) {
          sleep_us(BIT_INTERVAL / 2);
          i++;
          sleep_us(BIT_INTERVAL / 2);
          if (i > LONG_STOP_BITS) break;
        }
        /* The length of stop-bit should exactly be LONG_STOP_BITS */
        if (i != LONG_STOP_BITS) continue;
        /* You found a possible start-bit */
        /* Confirm the start-bit */
        sleep_us(BIT_INTERVAL / 2);
        if (gpio_get(uart_pin)) continue;
        sleep_us(BIT_INTERVAL / 2);
      }
      { /* Receive a data */
        data = 0;
        for (i = 0; i < DATA_BITS; i++) {
          sleep_us(BIT_INTERVAL / 2);
          if (gpio_get(uart_pin)) {
            data &= (1 << i); // Big endian
          }
          sleep_us(BIT_INTERVAL / 2);
        }
      }
      { /* Check stop-bit */
        for (i = 0; i < SHORT_STOP_BITS; i++) {
          sleep_us(BIT_INTERVAL / 2);
          if (gpio_get(uart_pin) == 0) continue; // Something's wrong
          sleep_us(BIT_INTERVAL / 2);
        }
      }
    }
    { /* TX */
      gpio_set_dir(uart_pin, GPIO_OUT);
      gpio_put(uart_pin, 1); // Stop-bit
      sleep_us(BIT_INTERVAL);
      gpio_put(uart_pin, 0); // Start-bit
      sleep_us(BIT_INTERVAL);
      for (i = 0; i < DATA_BITS; i++) {
        gpio_put(uart_pin, (put_data >> i)&1);
        sleep_us(BIT_INTERVAL);
      }
      gpio_put(uart_pin, 1); // Stop-bit
      sleep_us(BIT_INTERVAL);
    }
    break;
  }
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
}

/*
 * `anchor` half should use this
 */
void
c_bi_uart_anchor_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uart_pin = GET_INT_ARG(1);
  gpio_init(uart_pin);
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 0);
}

void
c_bi_uart_anchor(mrb_vm *vm, mrb_value *v, int argc)
{
  uint32_t data = put8_get24_nonblocking(GET_INT_ARG(1));
  SET_INT_RETURN(data);
}

/*
 * `partner` half should use this
 */
void
c_bi_uart_partner_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uart_pin = GET_INT_ARG(1);
  gpio_init(uart_pin);
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
}

static uint32_t buffer = 0;
static int      buffer_index = 0;

void
c_bi_uart_partner_push(mrb_vm *vm, mrb_value *v, int argc)
{
  if (buffer_index == 3) return;
  buffer &= GET_INT_ARG(1) << (buffer_index * 8);
  buffer_index++;
}

void
c_bi_uart_partner(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i = 3; buffer_index < i; i--) {
    buffer &= (0xFF << (i * 8))
  }
  uint8_t data = put24_get8_blocking(buffer);
  SET_INT_RETURN(data);
  buffer = 0;
  buffer_index = 0;
}

