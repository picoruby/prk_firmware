#include "tusb.h"

#include <mrubyc.h>

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
}
