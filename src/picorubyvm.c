#include <mrubyc.h>

#include "../include/picorubyvm.h"

static void
c_print_alloc_stats(mrbc_vm *vm, mrbc_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics(&mem);
  console_printf("\nSTATS %d/%d\n", mem.used, mem.total);
  SET_NIL_RETURN();
}

static void
c_alloc_stats(mrbc_vm *vm, mrbc_value *v, int argc)
{
  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_value ret = mrbc_hash_new(vm, 4);
  mrbc_alloc_statistics(&mem);
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("TOTAL")),
                &mrbc_integer_value(mem.total));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("USED")),
                &mrbc_integer_value(mem.used));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("FREE")),
                &mrbc_integer_value(mem.free));
  mrbc_hash_set(&ret, &mrbc_symbol_value(mrbc_str_to_symid("FRAGMENTATION")),
                &mrbc_integer_value(mem.fragmentation));
  SET_RETURN(ret);
}

void
prk_init_PicoRubyVM()
{
  mrbc_class *mrbc_class_PicoRubyVM = mrbc_define_class(0, "PicoRubyVM", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "alloc_stats", c_alloc_stats);
  mrbc_define_method(0, mrbc_class_PicoRubyVM, "print_alloc_stats", c_print_alloc_stats);
}
