/* PicoRuby */
#include <picorbc.h>
#include <picogem_init.c>

/* Raspi SDK */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "bsp/board.h"
#include "hardware/clocks.h"

#include "../include/version.h"

/* mrbc_class */
#include "../include/keyboard.h"
#include "../include/usb_descriptors.h"
#include "../include/ws2812.h"
#include "../include/rotary_encoder.h"
#include "../include/joystick.h"
#include "../include/sounder.h"

/* ruby */
/* ext */
#include "../build/mrb/object-ext.c"
/* tasks */
#include "../build/mrb/usb_task.c"

#ifdef PRK_NO_MSC
#include <keymap.c>
#endif


#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

/* extern in mruby-pico-compiler/include/debug.h */
int loglevel = LOGLEVEL_WARN;

/* class PicoRubyVM */

static void
c_print_alloc_stats(mrb_vm *vm, mrb_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  console_printf("\nSTATS %d/%d\n", mem.used, mem.total);
  tud_task();
  SET_NIL_RETURN();
}

static void
c_alloc_stats(mrb_vm *vm, mrb_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_value ret = mrbc_hash_new(vm, 4);
  mrbc_alloc_statistics(&mem);
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("TOTAL")),
                &mrbc_integer_value(mem.total));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("USED")),
                &mrbc_integer_value(mem.used));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("FREE")),
                &mrbc_integer_value(mem.free));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("FRAGMENTATION")),
                &mrbc_integer_value(mem.fragmentation));
  SET_RETURN(ret);
}

/* class Machine */

static void
c_reset_usb_boot(mrb_vm *vm, mrb_value *v, int argc)
{
  reset_usb_boot(0, 0);
}

static void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

static void
c_srand(mrb_vm *vm, mrb_value *v, int argc)
{
  srand(GET_INT_ARG(1));
}

static void
c_rand(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

int autoreload_state; /* from keyboard.h */

#ifndef PRK_NO_MSC

#ifndef NODE_BOX_SIZE
#define NODE_BOX_SIZE 50
#endif

#endif /* PRK_NO_MSC */

static void
prk_init_picoruby(void)
{
  /* CONST */
  mrbc_sym sym_id = mrbc_str_to_symid("SIZEOF_POINTER");
  mrbc_set_const(sym_id, &mrbc_integer_value(PICORBC_PTR_SIZE));
  sym_id = mrbc_str_to_symid("PRK_NO_MSC");
#ifdef PRK_NO_MSC
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
  /* class Machine */
  mrbc_class *mrbc_class_Machine = mrbc_define_class(0, "Machine", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_Machine, "reset_usb_boot", c_reset_usb_boot);
  mrbc_define_method(0, mrbc_class_Machine, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_Machine, "srand", c_srand);
  mrbc_define_method(0, mrbc_class_Machine, "rand", c_rand);
  /* class PicoRubyVM */
  mrbc_class *mrbc_class_PicoRubyVM = mrbc_define_class(0, "PicoRubyVM", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "alloc_stats", c_alloc_stats);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "print_alloc_stats", c_print_alloc_stats);
  /* class USB */
  prk_init_usb();
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
#ifdef PRK_NO_MSC
  mrbc_create_task(keymap, 0);
#endif
  mrbc_run();
  return 0;
}
