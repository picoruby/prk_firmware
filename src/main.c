/* PicoRuby compiler */
#include <picorbc.h>

/* Raspi SDK */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "bsp/board.h"
#include "hardware/clocks.h"

/* mrbc_class */
#include "msc_disk.h"
#include "gpio.h"
#include "usb_descriptors.h"
#include "uart.h"
#include "ws2812.h"
#include "rotary_encoder.h"
#include <sandbox.h>

/* ruby */
/* models */
#include "ruby/app/models/float_ext.c"
#include "ruby/app/models/keyboard.c"
#include "ruby/app/models/rotary_encoder.c"
#include "ruby/app/models/rgb.c"
#include "ruby/app/models/buffer.c"
#include "ruby/app/models/debounce.c"
/* tasks */
#include "ruby/app/tasks/usb_task.c"
#include "ruby/app/tasks/rgb_task.c"
#ifdef PRK_NO_MSC
#include "ruby/app/keymap.c"
#endif

void
c___reset_usb_boot(mrb_vm *vm, mrb_value *v, int argc)
{
  reset_usb_boot(0, 0);
}

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

mrbc_tcb *tcb_keymap;

#define KEYMAP_PREFIX      "puts;" /* Somehow scapegoat... */
#define KEYMAP_PREFIX_SIZE (sizeof(KEYMAP_PREFIX) - 1)
#define SUSPEND_TASK       "suspend_task"
#define MAX_KEYMAP_SIZE    (1024 * 10)

void
create_keymap_task(mrbc_tcb *tcb)
{
  hal_disable_irq();
  DirEnt entry;
  StreamInterface *si;
  ParserState *p = Compiler_parseInitState(NODE_BOX_SIZE);
  msc_findDirEnt("KEYMAP  RB ", &entry);
  uint8_t *keymap_rb = NULL;
  if (entry.Name[0] != '\0') {
    RotaryEncoder_reset();
    uint32_t fileSize = entry.FileSize;
    console_printf("Size of keymap.rb: %u\n", fileSize);
    if (fileSize < MAX_KEYMAP_SIZE) {
      keymap_rb = malloc(KEYMAP_PREFIX_SIZE + fileSize + 1);
      keymap_rb[KEYMAP_PREFIX_SIZE + fileSize] = '\0';
      memcpy(keymap_rb, KEYMAP_PREFIX, KEYMAP_PREFIX_SIZE);
      msc_loadFile(keymap_rb + KEYMAP_PREFIX_SIZE, &entry);
      si = StreamInterface_new(NULL, (char *)keymap_rb, STREAM_TYPE_MEMORY);
    } else {
      console_printf("Must be less than %d bytes!\n", MAX_KEYMAP_SIZE);
      si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
    }
  } else {
    console_printf("No keymap.rb found.\n");
    si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
  }
  if (!Compiler_compile(p, si, NULL)) {
    console_printf("Compiling keymap.rb failed!\n");
    Compiler_parserStateFree(p);
    StreamInterface_free(si);
    p = Compiler_parseInitState(NODE_BOX_SIZE);
    si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
    Compiler_compile(p, si, NULL);
  }
  if (keymap_rb) free(keymap_rb);
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
  tcb_keymap = tcb;
}

void
c_resume_keymap(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_resume_task(tcb_keymap);
}

void
c_suspend_keymap(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_suspend_task(tcb_keymap);
}

void
c_reload_keymap(mrb_vm *vm, mrb_value *v, int argc)
{
  create_keymap_task(tcb_keymap);
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

typedef struct picogems {
  char *name;
  void (*initializer)(void);
  const uint8_t *mrb;
  const uint8_t *task;
  bool required;
} picogems;

picogems gems[] = {
  {"keyboard",        NULL,               keyboard,       NULL,     false},
  {"debounce",        NULL,               debounce,       NULL,     false},
  {"rotaryr-encoder", init_RotaryEncoder, rotary_encoder, NULL,     false},
  {"rgb",             init_RGB,           rgb,            rgb_task, false},
  {"", NULL, NULL, NULL, false}
};

void
c_require(mrb_vm *vm, mrb_value *v, int argc)
{
  char *name = GET_STRING_ARG(1);
  if (!name) return;
  for (int i = 0; ; i++) {
    if (!gems[i].mrb) break;
    if (strcmp(name, gems[i].name)) {
      if (gems[i].initializer) gems[i].initializer();
      mrbc_load_model(gems[i].mrb);
      if (gems[i].task) mrbc_create_task(gems[i].task, 0);
    }
  }
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
  mrbc_define_method(0, mrbc_class_object, "__reset_usb_boot", c___reset_usb_boot);
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
  mrbc_load_model(float_ext);
  mrbc_load_model(rgb);
  mrbc_load_model(buffer);
  mrbc_load_model(debounce);
  mrbc_load_model(rotary_encoder);
  mrbc_load_model(keyboard);
  mrbc_create_task(usb_task, 0);
  mrbc_create_task(rgb_task, 0);
  create_sandbox();
  mrbc_define_method(0, mrbc_class_object, "autoreload_ready?", c_autoreload_ready_q);
#ifdef PRK_NO_MSC
  mrbc_create_task(keymap, 0);
#else
  mrbc_define_method(0, mrbc_class_object, "reload_keymap",     c_reload_keymap);
  mrbc_define_method(0, mrbc_class_object, "suspend_keymap",    c_suspend_keymap);
  mrbc_define_method(0, mrbc_class_object, "resume_keymap",     c_resume_keymap);
#endif
  autoreload_state = AUTORELOAD_READY;
  mrbc_run();
  return 0;
}
