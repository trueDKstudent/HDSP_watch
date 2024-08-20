/*
 * usi_driver.h
 *
 * Created: 01.08.2022 13:29:07
 *  Author: drspa
 */ 


#ifndef USI_DRIVER_H_
#define USI_DRIVER_H_
#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <util/delay.h>
#include "glcdfont.c"

#define WRITE_ADDR 0xD0
#define READ_ADDR 0xD1
#define CNT_TOP 0x18

void PortInit(void);
void PcIntInit(void);
void PcIntDeinit(void);

void PWMInit(void);
void SPIInit(void);
void SPISend(uint8_t byte);
void I2CInit(void);
void I2CStart(void);
void I2CRepStart(void);
void I2CStop(void);
void I2CSendByte(uint8_t byte);
uint8_t I2CReadByte(uint8_t ack);

#endif /* USI_DRIVER_H_ */