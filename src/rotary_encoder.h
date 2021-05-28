/* mruby/c VM */
#include <mrubyc.h>

void c_init_rotary_encoder(mrb_vm *vm, mrb_value *v, int argc);

void c_consume_rotation_rotary_encoder(mrb_vm *vm, mrb_value *v, int argc);

#define ROTARY_ENCODER_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "init_rotary_encoder",             c_init_rotary_encoder); \
  mrbc_define_method(0, mrbc_class_object, "consume_rotation_rotary_encoder", c_consume_rotation_rotary_encoder); \
} while (0)


