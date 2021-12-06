/* mruby/c VM */
#include <mrubyc.h>

void c_init_encoder(mrb_vm *vm, mrb_value *v, int argc);
void c_read_encoder(mrb_vm *vm, mrb_value *v, int argc);

void RotaryEncoder_reset(void);

#define ROTARY_ENCODER_INIT() do { \
  mrbc_class *mrbc_class_RotaryEncoder = mrbc_define_class(0, "RotaryEncoder", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "init_encoder",  c_init_encoder);  \
  mrbc_define_method(0, mrbc_class_RotaryEncoder, "read_encoder",  c_read_encoder);  \
} while (0)
