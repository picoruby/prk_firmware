#include <string.h>
#include <stdlib.h>

/* pico-sdk and tinyusb */
#include <pico/stdlib.h>
#include <hardware/irq.h>
#include <hardware/clocks.h>
#include <hardware/sync.h>
#include <tusb.h>

#include "../lib/picoruby/mrbgems/picoruby-io/src/hal/hal.h"

/* mruby/c */
#include <rrt0.h>

#define ALARM_IRQ 0

#ifndef MRBC_TICK_UNIT
#define MRBC_TICK_UNIT 1
#endif

#ifndef MRBC_NO_TIMER
struct repeating_timer timer;

bool
alarm_irq(struct repeating_timer *t)
{
  mrbc_tick();
  return true;
}

void
hal_init(void)
{
  add_repeating_timer_ms(MRBC_TICK_UNIT, alarm_irq, NULL, &timer);
  clocks_hw->sleep_en0 = 0;
  clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS
  | CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_BITS
  | CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS
  | CLOCKS_SLEEP_EN1_CLK_SYS_UART0_BITS
  | CLOCKS_SLEEP_EN1_CLK_PERI_UART0_BITS;
}

void
hal_enable_irq()
{
  irq_set_enabled(ALARM_IRQ, true);
}

void
hal_disable_irq()
{
  irq_set_enabled(ALARM_IRQ, false);
}

void
hal_idle_cpu()
{
  __wfi();
}

#else // MRBC_NO_TIMER

void
hal_idle_cpu()
{
  sleep_ms(MRBC_TICK_UNIT);
  mrbc_tick();
}
#endif

int hal_write(int fd, const void *buf, int nbytes)
{
  tud_task();
  tud_cdc_write(buf, nbytes);
  return tud_cdc_write_flush();
}

int hal_flush(int fd) {
  return tud_cdc_write_flush();
}

int
hal_read_available(void)
{
  if (tud_cdc_available()) {
    return 1;
  } else {
    return 0;
  }
}

int
hal_getchar(void)
{
  if (tud_cdc_available()) {
    return tud_cdc_read_char();
  } else {
    return -1;
  }
}

//================================================================
/*!@brief
  abort program

  @param s	additional message.
*/
void hal_abort(const char *s)
{
  if( s ) {
    hal_write(1, s, strlen(s));
  }

  abort();
}


