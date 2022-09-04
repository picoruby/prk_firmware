#include <mrubyc.h>

void c_sounder_init(mrb_vm *vm, mrb_value *v, int argc);
void c_sounder_add_note(mrb_vm *vm, mrb_value *v, int argc);
void c_sounder_replay(mrb_vm *vm, mrb_value *v, int argc);
void c_sounder_clear_song(mrb_vm *vm, mrb_value *v, int argc);

#define SOUNDER_INIT() do {\
  mrbc_class *mrbc_class_Sounder = mrbc_define_class(0, "Sounder", mrbc_class_object); \
  mrbc_define_method(0, mrbc_class_Sounder, "sounder_init",   c_sounder_init);       \
  mrbc_define_method(0, mrbc_class_Sounder, "sounder_replay", c_sounder_replay);     \
  mrbc_define_method(0, mrbc_class_Sounder, "add_note",       c_sounder_add_note);   \
  mrbc_define_method(0, mrbc_class_Sounder, "clear_song",     c_sounder_clear_song); \
} while (0)
