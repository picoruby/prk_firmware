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

/* mrbc_class */
#include "../include/msc_disk.h"
#include "../include/gpio.h"
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
#include "picoruby-prk-rgb/include/prk-rgb.h"
#include "picoruby-prk-rgb/task/rgb_task.c"
#ifdef PRK_NO_MSC
#include <keymap.c>
#endif


#define MEMORY_SIZE (1024*200)

static uint8_t memory_pool[MEMORY_SIZE];

/* extern in mruby-pico-compiler/include/debug.h */
int loglevel = LOGLEVEL_WARN;

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t desc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,
  // Use Interface Association Descriptor (IAD) for CDC
  // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
  .bDeviceClass       = TUSB_CLASS_MISC,
  .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
  .bDeviceProtocol    = MISC_PROTOCOL_IAD,
  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
  /*
   * VID and PID from USB-IDs-for-free.txt
   * https://github.com/obdev/v-usb/blob/releases/20121206/usbdrv/USB-IDs-for-free.txt#L128
   */
  .idVendor           = 0x16c0,
  .idProduct          = 0x27db,
  .bcdDevice          = 0x0100,
  .iManufacturer      = 0x01,
  .iProduct           = 0x02,
  .iSerialNumber      = 0x03,
  .bNumConfigurations = 0x01
};

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+
#include "../include/version.h"
char const *string_desc_arr[STRING_DESC_ARR_SIZE] =
{
  (const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
  "PRK Firmware developers",     // 1: Manufacturer
  "Default VID/PID",             // 2: Product
  PRK_SERIAL,                    // 3: Serial
  "PRK CDC",                     // 4: CDC Interface
  "PRK MSC",                     // 5: MSC Interface
};
/*
 * If you want users to use the VIA feature, provide the `prk-conf.txt`
 * file in order to VIA/Remap can determine the layout of the keyboard.
 * Format of the content `prk-conf.txt` (at the very top of the file):
 *   0x1234:0xABCD:productName
 *   ^^^^^^ ^^^^^^ ^^^^^^^^^^^
 *    VID    PID      Name
 *   - Length of productName should be less than or equal 32 bytes
 * and any other letter must not be included in the file.
 */
#define PRK_CONF_MAX_LENGTH (7 + 7 + 33 + 2)
static void
configure_prk(void)
{
  static char prk_conf[PRK_CONF_MAX_LENGTH] = {0};
  DirEnt entry;
  msc_findDirEnt("PRK-CONFTXT", &entry);
  if (entry.Name[0] != '\0') {
    if (entry.FileSize > PRK_CONF_MAX_LENGTH) return;
    msc_loadFile(prk_conf, &entry);
    char *tok = strtok(prk_conf, ":");
    for (int i = 0; i < 3; i++) {
      if (tok == NULL) break;
      switch (i) {
        case 0:
          desc_device.idVendor  = (uint16_t)strtol(tok, NULL, 16);
          tok = strtok(NULL, ":");
          break;
        case 1:
          desc_device.idProduct = (uint16_t)strtol(tok, NULL, 16);
          tok = strtok(NULL, ": \t\n\r");
          break;
        case 2:
          string_desc_arr[2] = (const char *)tok;
          break;
      }
    }
  }
}

/* class PicoRubyVM */

static void
quick_print_alloc_stats()
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  console_printf("\nSTATS %d/%d\n", mem.used, mem.total);
  tud_task();
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

int autoreload_state; /* from msc_disk.h */

#ifndef PRK_NO_MSC

#ifndef NODE_BOX_SIZE
#define NODE_BOX_SIZE 50
#endif

#define KEYMAP_PREFIX        "begin\n"
#define KEYMAP_PREFIX_SIZE   (sizeof(KEYMAP_PREFIX) - 1)
#define KEYMAP_POSTFIX       "\nrescue => e\nputs e.class, e.message, 'Task stopped!'\nend"
#define KEYMAP_POSTFIX_SIZE  (sizeof(KEYMAP_POSTFIX))
#define SUSPEND_TASK         "while true;sleep 5;puts 'Please move keymap.rb into PRKFirmware drive';end;"
#define MAX_KEYMAP_SIZE      (1024 * 10)

static void
reset_vm(mrbc_vm *vm)
{
  vm->cur_irep        = vm->top_irep;
  vm->inst            = vm->cur_irep->inst;
  vm->cur_regs        = vm->regs;
  vm->target_class    = mrbc_class_object;
  vm->callinfo_tail   = NULL;
  vm->ret_blk         = NULL;
  vm->exception       = mrbc_nil_value();
  vm->flag_preemption = 0;
  vm->flag_stop       = 0;
}

mrbc_tcb*
create_keymap_task(mrbc_tcb *tcb)
{
  hal_disable_irq();
  DirEnt entry;
  StreamInterface *si;
  ParserState *p = Compiler_parseInitState(NULL, NODE_BOX_SIZE);
  msc_findDirEnt("KEYMAP  RB ", &entry);
  static uint8_t *keymap_rb = NULL;
  if (keymap_rb) picorbc_free(keymap_rb);
  if (entry.Name[0] != '\0') {
    RotaryEncoder_reset();
    uint32_t fileSize = entry.FileSize;
    console_printf("keymap.rb size: %u\n", fileSize);
    tud_task();
    if (fileSize < MAX_KEYMAP_SIZE) {
      keymap_rb = picorbc_alloc(KEYMAP_PREFIX_SIZE + fileSize + KEYMAP_POSTFIX_SIZE);
      memcpy(keymap_rb, KEYMAP_PREFIX, KEYMAP_PREFIX_SIZE);
      msc_loadFile(keymap_rb + KEYMAP_PREFIX_SIZE, &entry);
      memcpy(keymap_rb + KEYMAP_PREFIX_SIZE + fileSize, KEYMAP_POSTFIX, KEYMAP_POSTFIX_SIZE);
      si = StreamInterface_new(NULL, (char *)keymap_rb, STREAM_TYPE_MEMORY);
    } else {
      console_printf("Must be less than %d bytes!\n", MAX_KEYMAP_SIZE);
      tud_task();
      si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
    }
  } else {
    console_printf("No keymap.rb found!\n");
    tud_task();
    si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
  }
  uint8_t *vm_code = NULL;
  if (Compiler_compile(p, si, NULL)) {
    vm_code = p->scope->vm_code;
    p->scope->vm_code = NULL;
  } else {
    console_printf("Compiling keymap.rb failed!\n");
    tud_task();
    Compiler_parserStateFree(p);
    StreamInterface_free(si);
    p = Compiler_parseInitState(NULL, NODE_BOX_SIZE);
    si = StreamInterface_new(NULL, SUSPEND_TASK, STREAM_TYPE_MEMORY);
    Compiler_compile(p, si, NULL);
  }
  quick_print_alloc_stats();
  Compiler_parserStateFree(p);
  StreamInterface_free(si);
  if (tcb == NULL) {
    tcb = mrbc_create_task(vm_code, 0);
  } else {
    mrbc_suspend_task(tcb);
    if(mrbc_load_mrb(&tcb->vm, vm_code) != 0) {
      console_printf("Loading keymap.mrb failed!\n");
      tud_task();
    } else {
      reset_vm(&tcb->vm);
      mrbc_resume_task(tcb);
    }
  }
  autoreload_state = AUTORELOAD_WAIT;
  hal_enable_irq();
  return tcb;
}

#endif /* PRK_NO_MSC */

static void
prk_init_picoruby(void)
{
  /* CONST */
  mrbc_sym sym_id = mrbc_str_to_symid("SIZEOF_POINTER");
  mrbc_set_const(sym_id, &mrbc_integer_value(PICORBC_PTR_SIZE));
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
  /* GPIO */
  prk_init_gpio();
  /* class USB */
  prk_init_usb();
}

int
main(void)
{
  configure_prk();
  /* RP2 */
  stdio_init_all();
  board_init();
  tusb_init();
  /* PicoRuby */
  mrbc_init(memory_pool, MEMORY_SIZE);
  prk_init_picoruby();
  /* Tasks */
  mrbc_create_task(usb_task, 0);
  tcb_rgb = mrbc_create_task(rgb_task, 0);
#ifdef PRK_NO_MSC
  mrbc_create_task(keymap, 0);
#else
  prk_msc_init();
#endif
  mrbc_run();
  return 0;
}
