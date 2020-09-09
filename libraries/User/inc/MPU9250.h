#ifndef __MPU9250_H__
#define __MPU9250_H__

extern volatile uint8_t MPU_int_status;

void MPU9250_reg_read_byte(uint8_t reg, uint8_t *data);
void MPU9250_reg_read_arg(uint8_t reg, uint8_t size, uint8_t data[]);
void MPU9250_reg_write_byte(uint8_t reg, uint8_t data);
void MPU9250_reg_write_arg(uint8_t reg, uint8_t size, uint8_t data[]);

void MPU9250_init(void);
void MPU9250_reset(void);
void MPU9250_set_offset(int16_t []);
#endif //__MPU9250_H__
