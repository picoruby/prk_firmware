#include "i2c.h"
#include "oled.h"
#include "glcdfont.c"

#include "hardware/i2c.h"

void
move_line(uint8_t index)
{
  uint8_t datas[5];
  datas[0] = 0x00;
  datas[1] = 0xB0 + index;
  datas[2] = 0x21;
  datas[3] = 0x00;
  datas[4] = 0x7F;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 5, false);
}

void
c_oled_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint8_t datas[9];
  // display off
  datas[0] = 0x00;
  datas[1] = 0xAE;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // 64(0x3F + 0x01) lines
  datas[1] = 0xA8;
  datas[2] = 0x3F;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // Line move 0
  datas[1] = 0xD3;
  datas[2] = 0x00;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // start line setup
  datas[1] = 0x40;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // Set Segment re-map 0xA0/0xA1
  datas[1] = 0xA0;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // COM Output Scan Direction
  datas[1] = 0xC0;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // COM Pins hardware configuration
  datas[1] = 0xDA;
  datas[2] = 0x02;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // Set Contrast
  datas[1] = 0x81;
  datas[2] = 0xFF;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // Disable Display
  datas[1] = 0xA4;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // Set Normal Display
  datas[1] = 0xA6;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  // Set Display Clock
  datas[1] = 0xD5;
  datas[2] = 0x80;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // Page addressing mode
  datas[1] = 0x20;
  datas[2] = 0x10;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  datas[1] = 0x21;
  datas[2] = 0x00;
  datas[3] = 127;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 4, false);
  datas[1] = 0x22;
  datas[3] = 0x07;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 4, false);
  // charge pump regulator
  datas[1] = 0x8D;
  datas[2] = 0x14;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 3, false);
  // Display On
  datas[1] = 0xAF;
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
  int i = 0;
  int j = 0;
  while(i<8){
    datas[0] = 0x00;
    datas[1] = 0xB0 + i;
    i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 2, false);
    j = 0;
    datas[0] = 0x40;
    datas[1] = datas[2] = datas[3] = datas[4] = datas[5] = datas[6] = datas[7] = datas[8] = 0x00; 
    while(j<16){
      i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 9, false);
      j = j+1;
    }
    i = i+1;
  }
}

void
c_oled_puts_logo(mrb_vm *vm, mrb_value *v, int argc)
{
  unsigned char datas[21*6+1];
  datas[0] = 0x40;
  move_line(0);
  memcpy(&datas[1],&font[6*32*4],21*6);
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 21*6+1, false);
  move_line(1);
  memcpy(&datas[1],&font[6*32*5],21*6);
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 21*6+1, false);
  move_line(2);
  memcpy(&datas[1],&font[6*32*6],21*6);
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, 21*6+1, false);
}

void
c_oled_puts_txt(mrb_vm *vm, mrb_value *v, int argc)
{
  unsigned char datas[21*6+1];
  datas[0] = 0x40;
  uint8_t line = GET_INT_ARG(1);
  unsigned char *txt = mrbc_string_cstr(&v[2]);
  uint8_t txt_size = mrbc_string_size(&v[2]);
  move_line(line);
  for(int i=0; i <= txt_size; i++){
    memcpy(&datas[i*6+1],&font[6*txt[i]],6);
  }
  i2c_write_blocking(I2C_PORT, OLED_ADDR, datas, txt_size*6+1, false);
}
