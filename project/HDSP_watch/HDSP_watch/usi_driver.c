/*
 * usi_driver.c
 *
 * Created: 01.08.2022 13:24:52
 *  Author: drspa
 */ 

#include "usi_driver.h"

void PWM_init(void)
{
	TCCR1 |= (1<<CS12)|(1<<CS11)|(1<<CS10); // clk_t1/64
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS01); // clk_t0/8
	// clk_t1 = clk_t0 = 8 MHz	

	OCR0A = cnt_top;
	OCR0B = cnt_top/4;
}

void SPI_init(void)
{
	USICR = 0x00;
	USICR |= (1<<USIWM0);
	PORTB |= (1<<PB2)|(1<<PB3);
}

void SPI_send(uint8_t byte)
{
	uint8_t i=0;
	USIDR = byte;
	while (i<7) {
		USICR |= (1<<USITC);
		USICR |= (1<<USICLK)|(1<<USITC);
		i++;
	}
}

void I2C_init(void)
{
	USICR = 0x00;
	PORTB &= ~(1<<PB3);
	USICR |= (1<<USIWM1);
	USIDR = 0x80;
	PORTB |= (1<<PB0)|(1<<PB2);
	USISR |= (1<<USISIF);
}

void I2C_start(void)
{
	TCNT0 = cnt_top-1;
	USIDR = 0x80;
	USICR |= (1<<USICS0);
	_delay_us(10);
	USISR |= (1<<USISIF);
	USICR |= (1<<USITC);
	_delay_us(3);
}

void I2C_rep_start(void)
{
	TCNT0 = 0x00;
	USIDR = 0x80;
	_delay_us(3);
	USICR |= 0x01;
	_delay_us(5);
	TCNT0 = cnt_top - 1;
	_delay_us(6);
	USISR |= (1<<USISIF);
	USICR |= 0x01;
	_delay_us(3);
}

void I2C_stop(void)
{
	USICR |= 0x01;
	_delay_us(5);
	USICR &= ~(1<<USICS0);
	USIDR = 0x80;
	USISR |= (1<<USIPF);
	_delay_us(5);
}

void I2C_send_byte(uint8_t byte)
{
	USISR = 0x00;
	TCNT0 = 0x00;
	USIDR = byte;
	
	while((USISR & 0x09) != 0x09) {

		if((TIFR & 0x08) == 0x08){
			USICR |= 0x01;
			_delay_us(cnt_top/2);
			USICR |= 0x01;
			TIFR |= 0x08;
		}
		
		if((USISR & 0x08) == 0x08){
			DDRB &= 0xFE;
			PORTB &= 0xFE;
		}
	}
	DDRB |= 0x01;
	PORTB |= 0x01;
	USIDR = 0x00;
}

uint8_t I2C_read_byte(uint8_t ack)
{
	uint8_t byte=0, lock=0;
	USISR = 0x00;
	TCNT0 = 0x00;
	
	DDRB &= ~0x01;
	PORTB &= ~0x01;
	
	while((USISR & 0x09) != 0x09) {

		if((TIFR & 0x08) == 0x08){
			USICR |= 0x01;
			_delay_us(cnt_top/2+10);
			USICR |= 0x01;
			TIFR |= 0x08;
		}
		
		if((USISR & 0x08) == 0x08 && !lock){
			byte = USIDR;
			lock=1;
			
			DDRB |= 0x01;
			if(!ack) PORTB &= ~0x01;
			else USIDR |= 0x80, PORTB |= 0x01;
		}
	}
	
	if(ack) USISR |= (1<<USISIF);
	PORTB |= 0x01;
	USIDR = 0x00;

	return byte;
}
