#include "i2c.h"

#include "hardware/i2c.h"

void
c_i2c_init(mrb_vm *vm, mrb_value *v, int argc)
{
	i2c_init(i2c0, 400 * 1000);
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
	uint8_t rom_addr = GET_INT_ARG(2);
	uint8_t buffer_num = argc-2;
	uint8_t buffer[buffer_num];
	for(i=0;i <= buffer_num;i++){
    buffer[i] = GET_INT_ARG(i);
  }
	i2c_write_blocking(i2c0, slave_addr, buffer, buffer_num, false);
}

void
c_i2c_read(mrb_vm *vm, mrb_value *v, int argc)
{
	uint8_t slave_addr = GET_INT_ARG(1);
	uint8_t rom_addr = GET_INT_ARG(2);
	uint8_t buffer_num = GET_INT_ARG(3);
	uint8_t buffer[buffer_num];
  i2c_write_blocking(i2c0, slave_addr, &rom_addr, 1, true);
  i2c_read_blocking(i2c0, slave_addr, buffer, buffer_num, false);
}
