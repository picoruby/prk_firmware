/*! @file
  @brief
  Hardware abstraction layer
        for RP2040

  <pre>
  Copyright (C) 2016-2021 Kyushu Institute of Technology.
  Copyright (C) 2016-2021 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include <string.h>

/***** Local headers ********************************************************/
#include "hal.h"

/***** Constat values *******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
struct repeating_timer timer;

/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/
/***** Global functions *****************************************************/
#ifndef MRBC_NO_TIMER

//================================================================
/*!@brief
  timer alarm irq

*/
bool alarm_irq(struct repeating_timer *t) {
  mrbc_tick();
}

//================================================================
/*!@brief
  initialize

*/
void hal_init(void){
  add_repeating_timer_ms(1, alarm_irq, NULL, &timer);
  clocks_hw->sleep_en0 = 0;
  clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS
                        | CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_BITS
                        | CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS
                        | CLOCKS_SLEEP_EN1_CLK_SYS_UART0_BITS
                        | CLOCKS_SLEEP_EN1_CLK_PERI_UART0_BITS;
}

//================================================================
/*!@brief
  Write

  @param  fd    dummy, but 1.
  @param  buf   pointer of buffer.
  @param  nbytes        output byte length.

  Memo: Steps to use uart_putc_raw() with hal_write.
  1. Write in main function↓
    uart_init(uart0,115200);
    gpio_set_function(0,GPIO_FUNC_UART);
    gpio_set_function(1,GPIO_FUNC_UART);

  2. Comment out the putchar for hal_write.
  3. Uncomment uart_putc_raw for hal_write.
*/
//int hal_write(int fd, const void *buf, int nbytes)
//{
//  int i = nbytes;
//  const uint8_t *p = buf;
//
//  while( --i >= 0 ) {
//    putchar( *p++ );
//    // uart_putc_raw(uart0, *p++ );
//  }
//
//  return nbytes;
//}
/*
 * hal_write() using tinyusb
 */
#define CFG_TUSB_MCU 1
#include "tusb_config.h"
#include "tusb.h"
int hal_write(int fd, const void *buf, int nbytes)
{
  tud_cdc_write(buf, nbytes);
  tud_cdc_write_flush();
  return nbytes;
}

//================================================================
/*!@brief
  Flush write baffer

  @param  fd    dummy, but 1.
*/
int hal_flush(int fd) {
  return 0;
}

#endif /* ifndef MRBC_NO_TIMER */


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

