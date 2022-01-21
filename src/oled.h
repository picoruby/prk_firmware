/* mruby/c VM */
#include <mrubyc.h>

#define PROGMEM
#define OLED_ADDR 0x3c

void move_line(uint8_t index);

void c_oled_init(mrb_vm *vm, mrb_value *v, int argc);
void c_oled_puts_logo(mrb_vm *vm, mrb_value *v, int argc);
void c_oled_puts_txt(mrb_vm *vm, mrb_value *v, int argc);
void c_oled_clear(mrb_vm *vm, mrb_value *v, int argc);
void c_oled_clear_line(mrb_vm *vm, mrb_value *v, int argc);

#define OLED_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "oled_init",  c_oled_init);  \
  mrbc_define_method(0, mrbc_class_object, "oled_puts_logo", c_oled_puts_logo); \
  mrbc_define_method(0, mrbc_class_object, "oled_puts_txt", c_oled_puts_txt); \
  mrbc_define_method(0, mrbc_class_object, "oled_clear", c_oled_clear); \
  mrbc_define_method(0, mrbc_class_object, "oled_clear_line", c_oled_clear_line); \
} while (0)

