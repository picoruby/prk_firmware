#include "mrubyc.h"
#include <stdio.h>
#include <stdlib.h>
#include "models.c"
#include "test.c"

#define MEMORY_SIZE (1024*640)-1
static uint8_t my_memory_pool[MEMORY_SIZE];

int exit_code;

static void c_exit(mrb_vm *vm, mrb_value *v, int argc){
  exit_code = GET_INT_ARG(1);
}

//================================================================
/*! DEBUG PRINT
*/
static void c_debugprint(mrb_vm *vm, mrb_value *v, int argc){
  console_putchar('\n');
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
  unsigned char *message = GET_STRING_ARG(1);
  console_printf("%s\n", message);
  int total, used, free, fragment;
  mrbc_alloc_statistics( &total, &used, &free, &fragment );
  console_printf("Memory total:%d, used:%d, free:%d, fragment:%d\n", total, used, free, fragment );
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
  console_putchar('\n');
}

int main(void) {
  mrbc_init(my_memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "debugprint", c_debugprint);
  mrbc_define_method(0, mrbc_class_object, "exit", c_exit);
  mrbc_create_task( models, 0 );
  mrbc_create_task( test, 0 );
  mrbc_run();
  return exit_code;
}
