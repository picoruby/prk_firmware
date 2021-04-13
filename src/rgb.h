#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
//#include "pwm.h"

/* mruby/c VM */
#include <mrubyc.h>

#define RGB_INIT() do { \
  mrbc_define_method(0, mrbc_class_object, "tight_loop_contents", c_tight_loop_contents);  \
  mrbc_define_method(0, mrbc_class_object, "resume_rgb",          c_resume_rgb);  \
  mrbc_define_method(0, mrbc_class_object, "rgb_init",            c_rgb_init);  \
} while (0)

extern mrbc_tcb *tcb_rgb;

void c_rgb_init(mrb_vm *vm, mrb_value *v, int argc);

void c_resume_rgb(mrb_vm *vm, mrb_value *v, int argc);

void c_tight_loop_contents(mrb_vm *vm, mrb_value *v, int argc);

