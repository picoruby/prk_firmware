#include "mrubyc.h"
#include <stdio.h>
#include <stdlib.h>
#include "models.c"
#include "test.c"

#define MEMORY_SIZE (1024*640)-1
static uint8_t my_memory_pool[MEMORY_SIZE];

int exit_code = 0;

static void c_exit(struct VM *vm, mrbc_value v[], int argc)
{
  exit_code = GET_INT_ARG(1);
}

//================================================================
/*! Object#instance_variable_get
*/
static void c_instance_variable_get(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_kv_handle *kvh = &v[0].instance->ivar;
  const char *name = (const char *)GET_STRING_ARG(1);
  for(int i = 0; i < kvh->n_stored; i++) {
    if (strncmp(&name[1], symid_to_str(kvh->data[i].sym_id), strlen(name) - 1) == 0) {
      SET_RETURN(kvh->data[i].value);
      mrbc_incref(&kvh->data[i].value);
      return;
    }
  }
  SET_NIL_RETURN();
}

//================================================================
/*! DEBUG PRINT
*/
static void c_debugprint(struct VM *vm, mrbc_value v[], int argc)
{
  console_putchar('\n');
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
  unsigned char *message = GET_STRING_ARG(1);
  console_printf("%s\n", message);

  struct MRBC_ALLOC_STATISTICS mem;
  mrbc_alloc_statistics( &mem );
  console_printf("Memory total:%d, used:%d, free:%d, fragment:%d\n", mem.total, mem.used, mem.free, mem.fragmentation );
  for( int i = 0; i < 79; i++ ) { console_putchar('='); }
  console_putchar('\n');
  console_putchar('\n');
}

int main(void) {
  mrbc_init(my_memory_pool, MEMORY_SIZE);
  mrbc_define_method(0, mrbc_class_object, "debugprint", c_debugprint);
  mrbc_define_method(0, mrbc_class_object, "exit", c_exit);
  mrbc_define_method(0, mrbc_class_object, "instance_variable_get", c_instance_variable_get);
  mrbc_create_task( models, 0 );
  mrbc_create_task( test, 0 );
  mrbc_run();
  return exit_code;
}
