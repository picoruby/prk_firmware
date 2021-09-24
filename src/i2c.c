#include "i2c.h"

#include "hardware/i2c.h"

void
c_i2c_init(mrb_vm *vm, mrb_value *v, int argc)
{
	i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(16, GPIO_FUNC_I2C);
  gpio_set_function(17, GPIO_FUNC_I2C);
  gpio_pull_up(16);
  gpio_pull_up(17);
}

void
c_i2c_write(mrb_vm *vm, mrb_value *v, int argc)
{
	int i;
	uint8_t slave_addr = GET_INT_ARG(1);
  uint8_t n = mrbc_array_size(&v[2]);
  uint8_t datas[n];
	for(int i=0;i < n;i++){
	  datas[i] = mrbc_array_get(&v[2] ,i).i;
	}
  i2c_write_blocking(I2C_PORT, slave_addr, datas, n, false);
}

void
c_i2c_read(mrb_vm *vm, mrb_value *v, int argc)
{
	uint8_t slave_addr = GET_INT_ARG(1);
	uint8_t rom_addr = GET_INT_ARG(2);
	uint8_t buffer_num = GET_INT_ARG(3);
	uint8_t buffer[buffer_num];
  i2c_write_blocking(I2C_PORT, slave_addr, &rom_addr, 1, true);
  i2c_read_blocking(I2C_PORT, slave_addr, buffer, buffer_num, false);

  mrbc_value datas = mrbc_array_new(vm, buffer_num);
  for(int j=0;j < buffer_num;j++){
    mrbc_value v = mrb_fixnum_value(buffer[j]);
    mrbc_array_set(&datas,j,&v);
  }
  SET_RETURN(datas);
}
