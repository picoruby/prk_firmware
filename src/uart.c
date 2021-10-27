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
 * (eg) 1-bit interval of 9600 bps is about 104 microseconds
 */
#define BIT_INTERVAL      (SAMPLING_INTERVAL * SAMPLING_COUNT)
#define SLEEP_MS         100
#define NIL              255
static int uart_pin;

//void
//core1_anchor_entry()
//{
uint8_t
anchor_getc()
{
  uint8_t data, i;
  { /* TX */
    { /* at least 2-bit long of stop-bit */
    //  sleep_us(BIT_INTERVAL * 2);
      /*
      * `if (i < BIT_INTERVAL * 1.5) continue;`
      * in core1_partner_entry() should catch this.
      */
    }
    { /* Start-bit */
      gpio_put(uart_pin, 0);
      sleep_us(BIT_INTERVAL);
    }
    { /* Send data */
      data = NIL;
      for (i = 0; i < DATA_BITS; i++) {
        gpio_put(uart_pin, (data >> i)&1);
        sleep_us(BIT_INTERVAL);
      }
    }
    { /* Stop-bit */
      gpio_put(uart_pin, 1);
      sleep_us(BIT_INTERVAL);
    }
  }
  bool error = false;
  { /* RX */
    { /* Wait until finding a stop-bit */
      gpio_set_dir(uart_pin, GPIO_IN);
      gpio_pull_up(uart_pin);
      //while (gpio_get(uart_pin));
      /* You are on a possible start-bit */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin)) error = true;
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
    { /* Check the stop-bit */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin) == 0) data = NIL; // Something's wrong
      sleep_us(BIT_INTERVAL / 2);
    }
  }
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 1);
  if (error) return NIL;
  return data;
}

//void
//core1_partner_entry()
//{
void
partner_putc(uint8_t put_data)
{
  uint8_t data, i;
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
  for (;;) {
    { /* RX */
      { /* Wait until finding a stop-bit long enough */
        i = 0;
        while (gpio_get(uart_pin)) {
          sleep_us(SAMPLING_INTERVAL);
          if (i < 255) i++;
        }
        if (i < SAMPLING_COUNT * 9) continue;
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
        sleep_us(BIT_INTERVAL / 2);
        if (gpio_get(uart_pin) == 0) continue; // Something's wrong
        sleep_us(BIT_INTERVAL / 2);
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
    /* RX again */
//    gpio_set_dir(uart_pin, GPIO_IN);
//    gpio_pull_up(uart_pin);
//    sleep_ms(SLEEP_MS);
}

/*
 * `anchor` half should use this
 */
void
c_bi_uart_rx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uart_pin = GET_INT_ARG(1);
  gpio_init(uart_pin);
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 1);
}

/*
 * `partner` half should use this
 */
void
c_bi_uart_tx_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uart_pin = GET_INT_ARG(1);
  gpio_init(uart_pin);
//  multicore_launch_core1(core1_partner_entry);
}

void
c_bi_uart_putc_raw(mrb_vm *vm, mrb_value *v, int argc)
{
  //if (!multicore_fifo_wready()) multicore_fifo_drain();
  //multicore_fifo_push_blocking(GET_INT_ARG(1));
  //multicore_launch_core1(core1_partner_entry);
  partner_putc(GET_INT_ARG(1));
}

void
c_bi_uart_getc(mrb_vm *vm, mrb_value *v, int argc)
{
  //multicore_reset_core1();
  //multicore_launch_core1(core1_anchor_entry);
  //if (multicore_fifo_rvalid()) {
  //  SET_INT_RETURN(multicore_fifo_pop_blocking());
  //} else {
  //  SET_NIL_RETURN();
  //}
  uint8_t data = anchor_getc();
  if (data == NIL) {
    SET_NIL_RETURN();
  } else {
    SET_INT_RETURN(data);
  }
}
