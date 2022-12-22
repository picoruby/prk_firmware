#include <mrubyc.h>
#include "pico/stdlib.h"
#include "picoruby-prk-keyboard/include/prk-keyboard.h"

#include "../include/usb_descriptors.h"
#include "../include/msc_disk.h"

#define DATA_BITS          8
#define SAMPLING_INTERVAL 10
#define SAMPLING_COUNT     8
/* (eg) 1-bit interval of 9600 bps is about 104 microseconds (usec) */
#define BIT_INTERVAL      (SAMPLING_INTERVAL * SAMPLING_COUNT)
#define LONG_STOP_BITS     8
#define SHORT_STOP_BITS    2
#define IDLE_BITS          8

static int uart_pin;

void
Keyboard_uart_anchor_init(uint32_t pin)
{
  uart_pin = pin;
  gpio_init(uart_pin);
  gpio_set_dir(uart_pin, GPIO_OUT);
  gpio_put(uart_pin, 0);
}

void
Keyboard_uart_partner_init(uint32_t pin)
{
  uart_pin = pin;
  gpio_init(uart_pin);
  gpio_set_dir(uart_pin, GPIO_IN);
  gpio_pull_up(uart_pin);
}

static void
__not_in_flash_func(c_Keyboard_uart_anchor)(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8_t data = (uint8_t)GET_INT_ARG(1);
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
  //if (error || data24 == 0) return NIL;
  if (error || data24 == 0) SET_INT_RETURN(NIL);
  //return data24;
  SET_INT_RETURN(data24);
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
__not_in_flash_func(Keyboard_mutual_partner_get8_put24_blocking)(uint32_t data24)
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

static void c_autoreload_ready_q(mrb_vm *vm, mrb_value *v, int argc)
{
  if (autoreload_state == AUTORELOAD_READY) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}

void
Keyboard_init_sub(mrbc_class *mrbc_class_Keyboard)
{
  mrbc_define_method(0, mrbc_class_Keyboard, "uart_anchor", c_Keyboard_uart_anchor);
  mrbc_define_method(0, mrbc_class_Keyboard, "autoreload_ready?", c_autoreload_ready_q);
#ifdef PRK_NO_MSC
  autoreload_state = AUTORELOAD_NONE;
#else
  autoreload_state = AUTORELOAD_READY;
#endif
}
