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

#define SAMPLING_INTERVAL 10
#define SAMPLING_COUNT     8
/* (eg) 1-bit interval of 9600 bps is about 104 microseconds (usec) */
#define BIT_INTERVAL      (SAMPLING_INTERVAL * SAMPLING_COUNT)
#define LONG_STOP_BITS     8
#define SHORT_STOP_BITS    2
#define IDLE_BITS          8
#define NIL         0xFFFFFF
static int uart_pin;
static bool mutual = false;

/*
 * Mutual UART on a single line may break your microcontroller.
 */
void
c_mutual_uart_at_my_own_risk_eq(mrb_vm *vm, mrb_value *v, int argc)
{
  if (GET_ARG(1).tt == MRBC_TT_TRUE) {
    mutual = true;
    console_printf("Caution: Using mutual UART is at your own risk.\n");
  } else {
    mutual = false;
  }
}

void
c_uart_anchor_init(mrb_vm *vm, mrb_value *v, int argc)
{
  if (mutual) {
    uart_pin = GET_INT_ARG(1);
    gpio_init(uart_pin);
    gpio_set_dir(uart_pin, GPIO_OUT);
    gpio_put(uart_pin, 0);
  } else {
    uart_init(UART_ID, 2400);
    gpio_set_function(GET_INT_ARG(1), GPIO_FUNC_UART); // Set the RX pin
    uart_set_baudrate(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false); // Turn UART flow control off
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Set data format
    uart_set_fifo_enabled(UART_ID, true); // Turn on FIFO
  }
}

/*
 * It seems RX pin can't be inverted to TX on Raspi Pico.
 * That's why using PIO instead.
 */
static PIO pio = pio0;
static uint sm = 0;
static uint offset;
void
c_uart_partner_init(mrb_vm *vm, mrb_value *v, int argc)
{
  if (mutual) {
    uart_pin = GET_INT_ARG(1);
    gpio_init(uart_pin);
    gpio_set_dir(uart_pin, GPIO_IN);
    gpio_pull_up(uart_pin);
  } else {
    offset = pio_add_program(pio, &uart_tx_program);
    uart_tx_program_init(pio, sm, offset, GET_INT_ARG(1), BAUD_RATE);
  }
}

/*
 * mutual == false
 */
uint32_t
oneway_anchor_get24(void)
{
  uint32_t data24 = 0;
  for (int i = 0; i < 24; i += 8) {
    if (uart_is_readable(UART_ID) == 0) {
      data24 |= 0xFF << i;
    } else {
      data24 |= uart_getc(UART_ID) << i;
    }
  }
  /* Getting rid of surpluses */
  while (uart_is_readable(UART_ID)) uart_getc(UART_ID);
  return data24;
}

void
oneway_partner_put24(uint32_t data24)
{
  uint8_t data1 = data24 & 0xFF;;
  uint8_t data2 = (data24 >> 8) & 0xFF;
  uint8_t data3 = data24 >> 16;
  if (data1 != 0xFF)
    pio_sm_put_blocking(pio, sm, data1);
  if (data1 != data2)
    pio_sm_put_blocking(pio, sm, data2);
  if (data2 != data3 && data1 != data3)
    pio_sm_put_blocking(pio, sm, data3);
}

/*
 * mutual == true
 */

uint32_t
mutual_anchor_put8_get24_nonblocking(uint8_t data)
{
  uint8_t i;
  { /* TX */
    { /* Stop-bit (length: LONG_STOP_BITS) */
      gpio_put(uart_pin, 1);
      sleep_us(BIT_INTERVAL * LONG_STOP_BITS);
    }
    { /* Start-bit (length: 1) */
      gpio_put(uart_pin, 0);
      sleep_us(BIT_INTERVAL);
    }
    { /* Send data */
      for (i = 0; i < DATA_BITS; i++) {
        gpio_put(uart_pin, (data >> i)&1);
        sleep_us(BIT_INTERVAL);
      }
    }
    { /* Stop-bit (length: SHORT_STOP_BITS) */
      gpio_put(uart_pin, 1);
      sleep_us(BIT_INTERVAL * SHORT_STOP_BITS);
    }
    { /* Extra-bit (length: 1) */
      gpio_put(uart_pin, 0);
      sleep_us(BIT_INTERVAL);
    }
  }
  /* Switch to RX */
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
  /* DMZ (length: 2) */
  sleep_us(BIT_INTERVAL * 2);
  /* Extra DMZ only for anchor (length: 2) */
  sleep_us(BIT_INTERVAL * 2);
  bool error = false;
  uint32_t data24 = 0;
  { /* RX */
    { /* Idle-bit (length: IDLE_BITS - 2) */
      for (i = 0; i < SAMPLING_COUNT * IDLE_BITS; i++) {
        sleep_us(SAMPLING_INTERVAL);
        if (gpio_get(uart_pin)) break;
      }
      if (i < SAMPLING_COUNT * (IDLE_BITS - 2)) {
        error = true;
        //console_printf("error 1 i: %d\n", i); ignorable...?
      }
      if (    SAMPLING_COUNT * (IDLE_BITS) < i) {
        error = true;
        console_printf("error 2 i: %d\n", i);
      }
    }
    { /* Stop-bit (length: 1) */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin) == 0) { error = true; }
      sleep_us(BIT_INTERVAL / 2);
    }
    { /* Start-bit (length: 1) */
      sleep_us(BIT_INTERVAL / 2);
      if (gpio_get(uart_pin) == 1) { error = true; }
      sleep_us(BIT_INTERVAL / 2);
    }
    { /* Receive a data of 24 bits (==8*3) */
      sleep_us(BIT_INTERVAL / 2);
      for (i = 0; i < DATA_BITS * 3; i++) {
        if (gpio_get(uart_pin)) {
          data24 |= (1 << i); // Big endian
        }
        sleep_us(BIT_INTERVAL);
      }
    }
    { /* Check the stop-bit */
      if (gpio_get(uart_pin) == 0) {
        error = true;
        console_printf("error 3\n");
      }
    }
  }
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 0); // Idling
  if (error || data24 == 0) return NIL;
  return data24;
}
/*
 * Data sent from anchor to partner
 *----------------------------------------------
 *              data      length
 *-----------------------------------------------
 *   Stop-bit : 11111111 LONG_STOP_BITS
 *   Start-bit: 0        1
 *   Data     : 87654321 DATA_BITS
 *   Stop-bit : 11       SHORT_STOP_BITS
 *   Extra-bit: 0        1
 *----------------------------------------------
 * Demilitarized zone (both are RX)
 *   DMZ      : N/A      4 for anchor / 2 for partner
 * Exchanging the role
 * Data sent from partner to anchor
 *----------------------------------------------
 *              data     length
 *-----------------------------------------------
 *   Idle-bit : 0000     IDLE_BITS
 *   Stop-bit : 1        1
 *   Start-bit: 0        1
 *   Data1    : 87654321 DATA_BITS
 *   Data2    : 87654321 DATA_BITS
 *   Data3    : 87654321 DATA_BITS
 *   Stop-bit : 1        1
 * Exchanging the role
 * Data sent from anchor to partner
 *----------------------------------------------
 *              data      length
 *-----------------------------------------------
 *   Idling   : 0        Until invoked
 */
uint8_t
mutual_partner_get8_put24_blocking(uint32_t data24)
{
  uint8_t data, i;
  for (;;) {
    { /* RX */
      { /* Wait until stop-bit appears */
        while (gpio_get(uart_pin) == 0);
        i = 0;
        while (gpio_get(uart_pin)) {
          i++;
          sleep_us(SAMPLING_INTERVAL);
        }
        /* The length of the stop-bit should roughly eq to LONG_STOP_BITS */
        if (i + 5 < SAMPLING_COUNT * LONG_STOP_BITS        ) { console_printf("e-1 i: %d\n",i); continue; }
        if (        SAMPLING_COUNT * LONG_STOP_BITS < i - 5) { console_printf("e-2 i: %d\n",i); continue; }
        /* You found a possible start-bit */
        /* Confirm the start-bit */
        sleep_us(BIT_INTERVAL / 2);
        if (gpio_get(uart_pin)) { console_printf("e-3\n"); continue;}
        sleep_us(BIT_INTERVAL / 2);
      }
      { /* Receive a data */
        data = 0;
        for (i = 0; i < DATA_BITS; i++) {
          sleep_us(BIT_INTERVAL / 2);
          if (gpio_get(uart_pin)) data |= (1 << i); // Big endian
          sleep_us(BIT_INTERVAL / 2);
        }
      }
      { /* Check stop-bit (length: SHORT_STOP_BITS) */
        i = 0;
        for (int j = 0; j < SAMPLING_COUNT * (SHORT_STOP_BITS + 1); j++) {
          sleep_us(SAMPLING_INTERVAL);
          if (gpio_get(uart_pin)) i++;
        }
        /* The length of the stop-bit should roughly eq to SHORT_STOP_BITS */
        if (i + 2 < SAMPLING_COUNT * SHORT_STOP_BITS        ) { console_printf("e-4 i: %d\n",i); continue; }
        if (        SAMPLING_COUNT * SHORT_STOP_BITS < i - 2) { console_printf("e-5 i: %d\n",i); continue; }
      }
    }
    /* Extra-bit */
    sleep_us(BIT_INTERVAL);
    /* DMZ (length: 2) */
    sleep_us(BIT_INTERVAL * 2);
    { /* TX (without Stop-bit before Start-bit) */
      gpio_set_dir(uart_pin, GPIO_OUT);
      gpio_put(uart_pin, 0); // Idle-bit
      sleep_us(BIT_INTERVAL * IDLE_BITS);
      gpio_put(uart_pin, 1); // Stop-bit
      sleep_us(BIT_INTERVAL);
      gpio_put(uart_pin, 0); // Start-bit
      sleep_us(BIT_INTERVAL);
      for (i = 0; i < DATA_BITS * 3; i++) {
        gpio_put(uart_pin, (data24 >> i)&1);
        sleep_us(BIT_INTERVAL);
      }
      gpio_put(uart_pin, 1); // Stop-bit
      sleep_us(BIT_INTERVAL);
    }
    break;
  }
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
  return data;
}

void
c_uart_anchor(mrb_vm *vm, mrb_value *v, int argc)
{
  uint32_t data;
  if (mutual) {
    data = mutual_anchor_put8_get24_nonblocking(GET_INT_ARG(1));
  } else {
    data = oneway_anchor_get24();
  }
  SET_INT_RETURN(data);
}

static uint32_t buffer = 0;
static int      buffer_index = 0;

void
c_uart_partner_push8(mrb_vm *vm, mrb_value *v, int argc)
{
  int keycode = GET_INT_ARG(1);
  if (buffer_index > 2) return;
  buffer |= keycode << (buffer_index * 8);
  buffer_index++;
}

void
c_uart_partner(mrb_vm *vm, mrb_value *v, int argc)
{
  switch (buffer_index) {
    case 0: buffer  = NIL; break;
    case 1: buffer |= 0xFFFF00; break;
    case 2: buffer |= 0xFF0000; break;
  }
  uint8_t data;
  if (mutual) {
    data = mutual_partner_get8_put24_blocking(buffer);
  } else {
    oneway_partner_put24(buffer);
    data = 0;
  }
  SET_INT_RETURN(data);
  buffer = 0;
  buffer_index = 0;
}

