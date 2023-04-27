/* PicoRuby */
#include <picorbc.h>
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
#include "../include/tcode_table.h"

/* ruby */
/* ext */
#include "../build/mrb/object-ext.c"
/* tasks */
#include "../build/mrb/usb_task.c"

#ifdef PICORUBY_NO_MSC
#include <keymap.c>
#endif


#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

/* extern in mruby-pico-compiler/include/debug.h */
int loglevel = LOGLEVEL_WARN;

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

#endif /* PICORUBY_NO_MSC */

static void
prk_init_picoruby(void)
{
  /* CONST */
  mrbc_sym sym_id = mrbc_str_to_symid("SIZEOF_POINTER");
  mrbc_set_const(sym_id, &mrbc_integer_value(PICORBC_PTR_SIZE));
  sym_id = mrbc_str_to_symid("PICORUBY_NO_MSC");
#ifdef PICORUBY_NO_MSC
  mrbc_set_const(sym_id, &mrbc_true_value());
#else
  mrbc_set_const(sym_id, &mrbc_false_value());
#endif
  mrbc_vm *vm = mrbc_vm_open(NULL);
  sym_id = mrbc_str_to_symid("PRK_DESCRIPTION");
  mrbc_value prk_desc = mrbc_string_new_cstr(vm, PRK_DESCRIPTION);
  mrbc_set_const(sym_id, &prk_desc);
  mrbc_raw_free(vm);
  /* class Object */
  picoruby_load_model(object_ext);
  picoruby_init_require();
  prk_init_Machine();
  prk_init_PicoRubyVM();
  prk_init_USB();
  prk_init_TcodeTable();
}

int
main(void)
{
  /* RP2 */
  stdio_init_all();
  board_init();
  /* PicoRuby */
  mrbc_init(memory_pool, MEMORY_SIZE);
  prk_init_picoruby();
  /* TinyUSB */
  tusb_init();
  /* Tasks */
  mrbc_create_task(usb_task, 0);
#ifdef PICORUBY_NO_MSC
  mrbc_create_task(keymap, 0);
#endif
  mrbc_run();
  return 0;
}
