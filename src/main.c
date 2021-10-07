/* PicoRuby compiler */
#include <picorbc.h>

/* Raspi SDK */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "hardware/clocks.h"

/* mrbc_class */
#include "msc_disk.h"
#include "gpio.h"
#include "usb_descriptors.h"
#include "uart.h"
#include "ws2812.h"
#include "rotary_encoder.h"
#include "../lib/picoruby/cli/sandbox.h"

/* ruby */
/* models */
#include "ruby/app/models/core.c"
#include "ruby/app/models/keyboard.c"
#include "ruby/app/models/mode_key.c"
#include "ruby/app/models/rotary_encoder.c"
#include "ruby/app/models/rgb.c"
#include "ruby/app/models/buffer.c"
/* tasks */
#include "ruby/app/tasks/usb_task.c"
#include "ruby/app/tasks/rgb_task.c"
#ifdef PRK_NO_MSC
#include "ruby/app/keymap.c"
#endif

void
c_board_millis(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

void
c_srand(mrb_vm *vm, mrb_value *v, int argc)
{
  srand(GET_INT_ARG(1));
}

void
c_rand(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

int autoreload_state; /* from msc_disk.h */

#ifndef PRK_NO_MSC

#ifndef NODE_BOX_SIZE
#define NODE_BOX_SIZE 50
#endif

mrbc_tcb *
create_keymap_task(mrbc_tcb *tcb)
{
  hal_disable_irq();
  DirEnt entry;
  char *program;
  StreamInterface *si;
  ParserState *p = Compiler_parseInitState(NODE_BOX_SIZE);
  msc_findDirEnt("KEYMAP  RB ", &entry);
  if (entry.Name[0] != '\0') {
    program = (char *)(FLASH_MMAP_ADDR + SECTOR_SIZE * (1 + entry.FstClusLO));
    si = StreamInterface_new(program, STREAM_TYPE_MEMORY);
  } else {
    si = StreamInterface_new("suspend_task", STREAM_TYPE_MEMORY);
  }
  if (!Compiler_compile(p, si)) {
    Compiler_parserStateFree(p);
    StreamInterface_free(si);
    p = Compiler_parseInitState(NODE_BOX_SIZE);
    si = StreamInterface_new("suspend_task", STREAM_TYPE_MEMORY);
    Compiler_compile(p, si);
  }
  if (tcb == NULL) {
    tcb = mrbc_create_task(p->scope->vm_code, 0);
  } else {
    mrbc_vm *vm = (mrbc_vm *)(&tcb->vm);
    int vm_id = vm->vm_id;
    mrbc_vm_end(vm);
    memset(vm, 0, sizeof(mrbc_vm));
    mrbc_load_mrb(vm, p->scope->vm_code);
    vm->vm_id = vm_id;
    mrbc_vm_begin(vm);
  }
  p->scope->vm_code = NULL;
  Compiler_parserStateFree(p);
  StreamInterface_free(si);
  autoreload_state = AUTORELOAD_WAIT;
  hal_enable_irq();
  mrbc_resume_task(tcb);
  return tcb;
}

mrbc_tcb *tcb_keymap;

void
c_suspend_keymap(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_suspend_task(tcb_keymap);
}

void
c_reload_keymap(mrb_vm *vm, mrb_value *v, int argc)
{
  tcb_keymap = create_keymap_task(tcb_keymap);
}

#endif /* PRK_NO_MSC */

void
c_autoreload_ready_q(mrb_vm *vm, mrb_value *v, int argc)
{
  if (autoreload_state == AUTORELOAD_READY) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}


#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

void
mrbc_load_model(const uint8_t *mrb)
{
  mrbc_vm *vm = mrbc_vm_open(NULL);
  if( vm == 0 ) {
    console_printf("Error: Can't open VM.\n");
    return;
  }
  if( mrbc_load_mrb(vm, mrb) != 0 ) {
    console_printf("Error: Illegal bytecode.\n");
    return;
  }
  mrbc_vm_begin(vm);
  mrbc_vm_run(vm);
  mrbc_raw_free(vm);
}

int loglevel;

int main() {
  loglevel = LOGLEVEL_WARN;

  stdio_init_all();
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "rand",         c_rand);
  mrbc_define_method(0, mrbc_class_object, "srand",        c_srand);
#ifndef PRK_NO_MSC
  msc_init();
#endif
  CDC_INIT();
  GPIO_INIT();
  USB_INIT();
  UART_INIT();
  WS2812_INIT();
  ROTARY_ENCODER_INIT();
  SANDBOX_INIT();
  mrbc_load_model(core);
  mrbc_load_model(rgb);
  mrbc_load_model(buffer);
  mrbc_load_model(rotary_encoder);
  mrbc_load_model(keyboard);
  mrbc_load_model(mode_key);
  mrbc_create_task(usb_task, 0);
  mrbc_create_task(rgb_task, 0);
  create_sandbox();
  mrbc_define_method(0, mrbc_class_object, "autoreload_ready?", c_autoreload_ready_q);
#ifdef PRK_NO_MSC
  mrbc_create_task(keymap, 0);
#else
  mrbc_define_method(0, mrbc_class_object, "reload_keymap",     c_reload_keymap);
  mrbc_define_method(0, mrbc_class_object, "suspend_keymap",    c_suspend_keymap);
  tcb_keymap = create_keymap_task(NULL);
#endif
  autoreload_state = AUTORELOAD_WAIT;
  mrbc_run();
  return 0;
}
