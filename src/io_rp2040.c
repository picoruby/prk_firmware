#include <stdlib.h>

#include <mrubyc.h>

#include "../lib/picoruby/mrbgems/picoruby-io-console/src/hal/hal.h"

void
c_raw_bang(mrb_vm *vm, mrb_value *v, int argc)
{
  // TODO
  SET_RETURN(v[0]);
}

void
c_cooked_bang(mrb_vm *vm, mrb_value *v, int argc)
{
  // TODO
  SET_RETURN(v[0]);
}

void
mrbc_io_rp2040_init(void)
{
  mrbc_class *class_IO = mrbc_define_class(0, "IO", mrbc_class_object);
  mrbc_define_method(0, class_IO, "raw!", c_raw_bang);
  mrbc_define_method(0, class_IO, "cooked!", c_cooked_bang);
}

