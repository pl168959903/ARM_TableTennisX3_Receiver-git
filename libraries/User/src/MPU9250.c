#include "TableTennisX3.h"
#include <stdio.h>

volatile uint8_t MPU_int_status = 0;

void MPU9250_reg_read_byte(uint8_t reg, uint8_t *data)
{
	SPI_SET_SS0_LOW(SPI0);
	
	SPI_WRITE_TX0(SPI0, 0x80 | reg);
	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));

	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	*data = SPI_READ_RX0(SPI0);
	
	SPI_SET_SS0_HIGH(SPI0);
}

void MPU9250_reg_read_arg(uint8_t reg, uint8_t size, uint8_t data[])
{
	uint8_t i;
	SPI_SET_SS0_LOW(SPI0);
	
	SPI_WRITE_TX0(SPI0, 0x80 | reg);
	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	NRF_status_reg = SPI_READ_RX0(SPI0);
	for(i=0;i<size;i++)
	{
		SPI_TRIGGER(SPI0);
		while(SPI_IS_BUSY(SPI0));
		data[i] = SPI_READ_RX0(SPI0);
	}
	SPI_SET_SS0_HIGH(SPI0);
}
void MPU9250_reg_write_byte(uint8_t reg, uint8_t data)
{
	SPI_SET_SS0_LOW(SPI0);
	
	SPI_WRITE_TX0(SPI0, reg);
	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	NRF_status_reg = SPI_READ_RX0(SPI0);
	
	SPI_WRITE_TX0(SPI0, data);
	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	
	SPI_SET_SS0_HIGH(SPI0);
}
void MPU9250_reg_write_arg(uint8_t reg, uint8_t size, uint8_t data[])
{
	uint8_t i;
	SPI_SET_SS0_LOW(SPI0);
	
	SPI_WRITE_TX0(SPI0, reg);
	SPI_TRIGGER(SPI0);
	while(SPI_IS_BUSY(SPI0));
	NRF_status_reg = SPI_READ_RX0(SPI0);
	for(i=0;i<size;i++)
	{
		SPI_WRITE_TX0(SPI0, data[i]);
		SPI_TRIGGER(SPI0);
		while(SPI_IS_BUSY(SPI0));
	}
	SPI_SET_SS0_HIGH(SPI0);
}
void MPU9250_init(void)
{
	MPU9250_reg_write_byte(USER_CTRL, 0x74);
	MPU9250_reg_write_byte(USER_CTRL, 0x72);
	MPU9250_reg_write_byte(USER_CTRL, 0x71);
	MPU9250_reg_write_byte(PWR_MGMT_2, 0x00);
	MPU9250_reg_write_byte(MPU_CONFIG, 0x03);
	MPU9250_reg_write_byte(SMPLRT_DIV, 0x18);
	MPU9250_reg_write_byte(GYRO_CONFIG, 0x18);
	MPU9250_reg_write_byte(ACCEL_CONFIG, 0x18);
	MPU9250_reg_write_byte(ACCEL_CONFIG2, 0x03);
	MPU9250_reg_write_byte(INT_PIN_CFG, 0xE0);
	MPU9250_reg_write_byte(INT_ENABLE, 0x01);
}
void MPU9250_reset(void)
{
	MPU9250_reg_write_byte(PWR_MGMT_1, 0x80);
	CLK_SysTickDelay(1000000);
}

void MPU9250_set_offset(int16_t data[])
{
	int16_t	offset[6] = {0};
	uint8_t temp[2];
	uint8_t i;
	MPU9250_reg_read_arg(XA_OFFSET_H, 2, temp);
	offset[0] = (int16_t)((temp[0]<<8) | temp[1]);
	MPU9250_reg_read_arg(YA_OFFSET_H, 2, temp);
	offset[1] = (int16_t)((temp[0]<<8) | temp[1]);
	MPU9250_reg_read_arg(ZA_OFFSET_H, 2, temp);
	offset[2] = (int16_t)((temp[0]<<8) | temp[1]);
	MPU9250_reg_read_arg(XG_OFFSET_H, 2, temp);
	offset[3] = (int16_t)((temp[0]<<8) | temp[1]);
	MPU9250_reg_read_arg(YG_OFFSET_H, 2, temp);
	offset[4] = (int16_t)((temp[0]<<8) | temp[1]);
	MPU9250_reg_read_arg(ZG_OFFSET_H, 2, temp);
	offset[5] = (int16_t)((temp[0]<<8) | temp[1]);
	
	for(i=0;i<6;i++)
	{
		offset[i] -= data[i];
		offset[i] &= 0xFFFE;
	}
	MPU9250_reg_write_byte(XA_OFFSET_H, (uint8_t)((offset[0]>>8) & 0x00FF));
	MPU9250_reg_write_byte(XA_OFFSET_L, (uint8_t)((offset[0]>>0) & 0x00FF));
	MPU9250_reg_write_byte(YA_OFFSET_H, (uint8_t)((offset[1]>>8) & 0x00FF));
	MPU9250_reg_write_byte(YA_OFFSET_L, (uint8_t)((offset[1]>>0) & 0x00FF));
	MPU9250_reg_write_byte(ZA_OFFSET_H, (uint8_t)((offset[2]>>8) & 0x00FF));
	MPU9250_reg_write_byte(ZA_OFFSET_L, (uint8_t)((offset[2]>>0) & 0x00FF));
}
