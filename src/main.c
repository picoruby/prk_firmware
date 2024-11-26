/* PicoRuby */
#include <picogem_init.c>

/* Raspi SDK */
#include <stdio.h>
#include <stdlib.h>
#include "bsp/board.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "../include/version.h"

/* mrbc_class */
#include "../include/picorubyvm.h"
#include "../include/machine.h"
#include "../include/keyboard.h"
#include "../include/usb_descriptors.h"
#include "../include/ws2812.h"
#include "../include/rotary_encoder.h"
#include "../include/joystick.h"
#include "../include/sounder.h"

/* tasks */
#include "usb_task.c"

#ifdef PICORUBY_NO_MSC
#include <keymap.c>
#endif


#if !defined(HEAP_SIZE)
  #if defined(PICO_RP2040)
    #if defined(PICORUBY_SQLITE3)
      #error "Not enough memory for SQLite3 in RP2040"
    #endif
    #define HEAP_SIZE (1024 * 194)
  #elif defined(PICO_RP2350)
    #if defined(PICORUBY_SQLITE3)
      #define HEAP_SIZE (1024 * (194 + 240))
    #else
      #define HEAP_SIZE (1024 * (194 + 260))
    #endif
  #else
    #error "Unknown board"
  #endif
#endif

static uint8_t memory_pool[HEAP_SIZE];

int autoreload_state; /* from keyboard.h */

#ifndef PICORUBY_NO_MSC

#ifndef NODE_BOX_SIZE
#define NODE_BOX_SIZE 50
#endif

void
tud_msc_write10_complete_cb(uint8_t lun)
{
  (void)lun;
  autoreload_state = AUTORELOAD_READY;
}

#endif /* !PICORUBY_NO_MSC */

static void
prk_init_picoruby(void)
{
  mrbc_vm *vm = mrbc_vm_open(NULL);
  /* CONST */
  mrbc_sym sym_id = mrbc_str_to_symid("PICORUBY_MSC");
  mrbc_value picoruby_msc = mrbc_string_new_cstr(vm,
#if defined(PICORUBY_NO_MSC)
    "NO_MSC"
#elif defined(PICORUBY_MSC_FLASH)
    "MSC_FLASH"
#elif defined(PICORUBY_MSC_SD)
    "MSC_SD"
#endif
  );
  mrbc_set_const(sym_id, &picoruby_msc);
  sym_id = mrbc_str_to_symid("PRK_DESCRIPTION");
  mrbc_value prk_desc = mrbc_string_new_cstr(vm, PRK_DESCRIPTION);
  mrbc_set_const(sym_id, &prk_desc);
  picoruby_init_require(vm);
  prk_init_Machine();
  prk_init_USB();
}

int
main(void)
{
  /* RP2 */
  stdio_init_all();
  board_init();
  /* PicoRuby */
  mrbc_init(memory_pool, HEAP_SIZE);
  prk_init_picoruby();
  /* Tasks */
  mrbc_create_task(usb_task, 0);
#ifdef PICORUBY_NO_MSC
  mrbc_create_task(keymap, 0);
#endif
  mrbc_run();
  return 0;
}
