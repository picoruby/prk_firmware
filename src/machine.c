#include <mrubyc.h>

#include <stdlib.h>
#include "pico/bootrom.h"
#include "bsp/board.h"

#include "../include/machine.h"

static void
c_delay_us(mrbc_vm *vm, mrbc_value *v, int argc)
{
  sleep_us(GET_INT_ARG(1));
}

static void
c_reset_usb_boot(mrbc_vm *vm, mrbc_value *v, int argc)
{
  reset_usb_boot(0, 0);
  SET_INT_RETURN(0);
}

static void
c_board_millis(mrbc_vm *vm, mrbc_value *v, int argc)
{
  SET_INT_RETURN(board_millis());
}

static void
c_srand(mrbc_vm *vm, mrbc_value *v, int argc)
{
  srand(GET_INT_ARG(1));
  SET_INT_RETURN(0);
}

static void
c_rand(mrbc_vm *vm, mrbc_value *v, int argc)
{
  SET_INT_RETURN(rand());
}

void
prk_init_Machine()
{
  mrbc_class *mrbc_class_Machine = mrbc_define_class(0, "Machine", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_Machine, "delay_us", c_delay_us);
  mrbc_define_method(0, mrbc_class_Machine, "reset_usb_boot", c_reset_usb_boot);
  mrbc_define_method(0, mrbc_class_Machine, "board_millis", c_board_millis);
  mrbc_define_method(0, mrbc_class_Machine, "srand", c_srand);
  mrbc_define_method(0, mrbc_class_Machine, "rand", c_rand);
}
