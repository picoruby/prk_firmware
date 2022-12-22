#include <mrubyc.h>
#include "picoruby-prk-rgb/include/prk-rgb.h"

mrbc_tcb *tcb_rgb = NULL;

static void
c_RGB_resume(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_resume_task(tcb_rgb);
  SET_TRUE_RETURN();
}

void
RGB_init_sub(mrbc_class *mrbc_class_RGB)
{
  mrbc_define_method(0, mrbc_class_RGB, "resume", c_RGB_resume);
}
