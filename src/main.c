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
#include "joystick.h"
#include <sandbox.h>

/* ruby */
/* models */
#include "ruby/app/models/float_ext.c"
#include "ruby/app/models/keyboard.c"
#include "ruby/app/models/rotary_encoder.c"
#include "ruby/app/models/rgb.c"
#include "ruby/app/models/buffer.c"
#include "ruby/app/models/via.c"
#include "ruby/app/models/debounce.c"
#include "ruby/app/models/joystick.c"
/* tasks */
#include "ruby/app/tasks/usb_task.c"
#include "ruby/app/tasks/rgb_task.c"
#ifdef PRK_NO_MSC
#include "ruby/app/keymap.c"
#endif

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
#include "version.h"
#define PRK_SERIAL (PRK_VERSION "-" PRK_BUILDDATE "-" PRK_REVISION)
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
 *   - IDs' prefix should be `0x`, should NOT be `0X`
 *   - Length of productName should be less than or equal 32 bytes
 * and any other letter must not be included in the file.
 */
#define PRK_CONF_LENGTH (7 + 7 + 32)
static void
configure_prk(void)
{
  DirEnt entry;
  uint8_t buf[PRK_CONF_LENGTH + 1] = {0};
  uint8_t vid[7] = {0};
  uint8_t pid[7] = {0};
  static char name[PRK_CONF_LENGTH - 14] = {0};
  msc_findDirEnt("PRK-CONFTXT", &entry);
  if (entry.Name[0] != '\0') {
    if (entry.FileSize > PRK_CONF_LENGTH) return;
    msc_loadFile(buf, &entry);
    if (strncmp("0x", buf, 2) || strncmp(":0x", buf + 6, 3)) return;
    memcpy(vid,  buf     , 6);
    memcpy(pid,  buf +  7, 6);
    memcpy(name, buf + 14, strlen(buf) - 14);
    for (int i = 0; ; i++) {
      if (name[i] == '\r' || name[i] == '\n') name[i] = '\0';
      if (name[i] == '\0') break;
    }
    desc_device.idVendor  = (uint16_t)strtol(vid, NULL, 16);
    desc_device.idProduct = (uint16_t)strtol(pid, NULL, 16);
    string_desc_arr[2] = (const char *)name;
  }
}

void
c_reset_usb_boot(mrb_vm *vm, mrb_value *v, int argc)
{
  reset_usb_boot(0, 0);
}

static void
quick_print_alloc_stats()
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  console_printf("\nSTATS %d/%d\n", mem.used, mem.total);
}

void
c_print_alloc_stats(mrb_vm *vm, mrb_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  console_printf("ALLOC STATS\n");
  console_printf(" TOTAL %6d\n", mem.total);
  console_printf(" USED  %6d\n", mem.used);
  console_printf(" FREE  %6d\n", mem.free);
  console_printf(" FRAG  %6d\n\n", mem.fragmentation);
  SET_NIL_RETURN();
}

void
c_alloc_stats(mrb_vm *vm, mrb_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  mrbc_value ret = mrbc_hash_new(vm, 4);
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

void
c_picorbc_ptr_size(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN(PICORBC_PTR_SIZE);
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
  quick_print_alloc_stats();
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

bool
mrbc_load_model(const uint8_t *mrb)
{
  mrbc_vm *vm = mrbc_vm_open(NULL);
  if( vm == 0 ) {
    console_printf("Error: Can't open VM.\n");
    return false;
  }
  if( mrbc_load_mrb(vm, mrb) != 0 ) {
    console_printf("Error: Illegal bytecode.\n");
    return false;
  }
  mrbc_vm_begin(vm);
  mrbc_vm_run(vm);
  mrbc_raw_free(vm);
  return true;
}

typedef struct picogems {
  const char *name;
  void (*initializer)(void);
  const uint8_t *mrb;
  const uint8_t *task;
  bool required;
} picogems;

static void
init_RotaryEncoder(void)
{
  ROTARY_ENCODER_INIT();
}

static void
init_RGB(void)
{
  WS2812_INIT();
}

static void
init_Joystick(void)
{
  JOYSTICK_INIT();
}

picogems gems[] = {
  {"keyboard",       NULL,               keyboard,       NULL,     false},
  {"debounce",       NULL,               debounce,       NULL,     false},
  {"rotary_encoder", init_RotaryEncoder, rotary_encoder, NULL,     false},
  {"rgb",            init_RGB,           rgb,            rgb_task, false},
  {"via",            NULL,               via,            NULL,     false},
  {"joystick",       init_Joystick,      joystick,        NULL,     false},
  {NULL, NULL, NULL, NULL, false}
};

void
c_require(mrb_vm *vm, mrb_value *v, int argc)
{
  char *name = GET_STRING_ARG(1);
  bool result = false;
  if (!name) return;
  for (int i = 0; ; i++) {
    if (gems[i].name == NULL) {
      console_printf("cannot load such mrb -- %s\n (LoadError)", name);
      break;
    } else if (strcmp(name, gems[i].name) == 0) {
      if (!gems[i].required) {
        if (gems[i].initializer) gems[i].initializer();
        if (mrbc_load_model(gems[i].mrb)) {
          if (gems[i].task) {
            if (mrbc_create_task(gems[i].task, 0) != NULL) {
              console_printf("failed to create task\n (LoadError)", name);
              result = false; /* ToDo: Exception */
            }
          }
          gems[i].required = true;
        } else {
          console_printf("failed to load mrb -- %s\n (LoadError)", name);
          result = false; /* ToDo: Exception */
        }
      }
      break;
    }
  }
  if (result) { SET_TRUE_RETURN(); } else { SET_FALSE_RETURN(); }
}

int loglevel;

int main() {
  loglevel = LOGLEVEL_WARN;

  configure_prk();

  stdio_init_all();
  board_init();
  tusb_init();
  mrbc_init(memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "require",      c_require);
  mrbc_define_method(0, mrbc_class_object, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_object, "rand",         c_rand);
  mrbc_define_method(0, mrbc_class_object, "srand",        c_srand);
  mrbc_define_method(0, mrbc_class_object, "picorbc_ptr_size", c_picorbc_ptr_size);
  mrbc_class *mrbc_class_Microcontroller = mrbc_define_class(0, "Microcontroller", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_Microcontroller, "reset_usb_boot",  c_reset_usb_boot);
  mrbc_class *mrbc_class_PicoRubyVM = mrbc_define_class(0, "PicoRubyVM", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "alloc_stats",       c_alloc_stats);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "print_alloc_stats", c_print_alloc_stats);
  mrbc_class *mrbc_class_Keyboard = mrbc_define_class(0, "Keyboard", mrbc_class_object);
#ifndef PRK_NO_MSC
  msc_init();
#endif
  CDC_INIT();
  GPIO_INIT();
  USB_INIT();
  UART_INIT();
  SANDBOX_INIT();
  mrbc_load_model(float_ext);
  mrbc_load_model(buffer);
  mrbc_load_model(keyboard);
  mrbc_create_task(usb_task, 0);
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
