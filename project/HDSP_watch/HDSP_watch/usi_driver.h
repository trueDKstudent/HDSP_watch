/*
 * usi_driver.h
 *
 * Created: 01.08.2022 13:29:07
 *  Author: drspa
 */ 


#ifndef USI_DRIVER_H_
#define USI_DRIVER_H_
#define F_CPU 8000000UL

#include <util/delay.h>
#include "glcdfont.c"

#define w_addr 0xD0
#define r_addr 0xD1
#define cnt_top 0x18

void PWM_init(void);
void SPI_init(void);
void SPI_send(uint8_t byte);
void I2C_init(void);
void I2C_start(void);
void I2C_rep_start(void);
void I2C_stop(void);
void I2C_send_byte(uint8_t byte);
uint8_t I2C_read_byte(uint8_t ack);

#endif /* USI_DRIVER_H_ */