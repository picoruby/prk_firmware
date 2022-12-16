#include "../tinyusb/tusb_config.h"
#include "tusb.h"

#include <mrubyc.h>
#include "../include/usb_cdc.h"

#ifndef CFG_TUSB_MCU
  #error CFG_TUSB_MCU must be defined
#endif

// Invoked when cdc when line state changed e.g connected/disconnected
void
tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
  (void) itf;
  (void) rts;

  // TODO set some indicator
  if ( dtr )
  {
    // Terminal connected
  }else
  {
    // Terminal disconnected
  }
}

// Invoked when CDC interface received data from host
void
tud_cdc_rx_cb(uint8_t itf)
{
  (void) itf;

  char buf[64];
  uint32_t count = tud_cdc_read(buf, sizeof(buf));
  tud_cdc_write(buf, count);
  tud_cdc_write_flush();
}
