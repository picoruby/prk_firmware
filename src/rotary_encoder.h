/* mruby/c VM */
#include <mrubyc.h>

void c_init_encoder(mrb_vm *vm, mrb_value *v, int argc);
void c_read_encoder(mrb_vm *vm, mrb_value *v, int argc);
void c_encoder_value(mrb_vm *vm, mrb_value *v, int argc);
void c_reset_encoder(mrb_vm *vm, mrb_value *v, int argc);


#define ROTARY_ENCODER_INIT() do { \
  mrbc_class *mrbc_class_RotaryEncoder = mrbc_define_class(0, "RotaryEncoder", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "init_encoder",  c_init_encoder);  \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "read_encoder",  c_read_encoder);  \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "encoder_value", c_encoder_value); \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "reset_encoder", c_reset_encoder); \
} while (0)
