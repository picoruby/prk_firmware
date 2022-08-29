#include <mrubyc.h>

void c_tone_init(mrb_vm *vm, mrb_value *v, int argc);
void c_tone_set_tones(mrb_vm *vm, mrb_value *v, int argc);
void c_tone_start(mrb_vm *vm, mrb_value *v, int argc);

#define TONE_INIT() do {\
  mrbc_class *mrbc_class_Tone = mrbc_define_class(0, "Tone", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Tone, "tone_init",      c_tone_init);  \
  mrbc_define_method(0, mrbc_class_Tone, "tone_start",     c_tone_start);  \
  mrbc_define_method(0, mrbc_class_Tone, "tone_set_tones", c_tone_set_tones);\
} while (0)
