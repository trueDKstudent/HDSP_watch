/*
 * usi_driver.c
 *
 * Created: 01.08.2022 13:24:52
 *  Author: drspa
 */ 

#include "usi_driver.h"

// Port initialization
void PortInit(void)
{
    CLKPR = 0x80;
    CLKPR = 0x00;
    DDRB = 0x0F;
}

void PcIntInit(void)
{
    GIMSK |= (1 << PCIE);
    PCMSK |= (1 << PCINT4); // enable interrupt from pin PC4
    sei(); // enable all interrupts
}

void PcIntDeinit(void)
{
    GIMSK = 0x00;
    PCMSK = 0x00;
    cli(); // disables all interrupts
}

void PWMInit(void)
{
	TCCR1 |= (1<<CS12)|(1<<CS11)|(1<<CS10); // clk_t1/64
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS01); // clk_t0/8
	// clk_t1 = clk_t0 = 8 MHz	

	OCR0A = CNT_TOP;
	OCR0B = CNT_TOP/4;
}

void SPIInit(void)
{
	USICR = 0x00;
	USICR |= (1<<USIWM0);
	PORTB |= (1<<PB2)|(1<<PB3);
}

void SPISend(uint8_t byte)
{
	uint8_t i=0;
	USIDR = byte;
	while (i<7) {
		USICR |= (1<<USITC);
		USICR |= (1<<USICLK)|(1<<USITC);
		i++;
	}
}

void I2CInit(void)
{
	USICR = 0x00;
	PORTB &= ~(1<<PB3);
	USICR |= (1<<USIWM1);
	USIDR = 0x80;
	PORTB |= (1<<PB0)|(1<<PB2);
	USISR |= (1<<USISIF);
}

void I2CStart(void)
{
	TCNT0 = CNT_TOP-1;
	USIDR = 0x80;
	USICR |= (1<<USICS0);
	_delay_us(10);
	USISR |= (1<<USISIF);
	USICR |= (1<<USITC);
	_delay_us(3);
}

void I2CRepStart(void)
{
	TCNT0 = 0x00;
	USIDR = 0x80;
	_delay_us(3);
	USICR |= 0x01;
	_delay_us(5);
	TCNT0 = CNT_TOP - 1;
	_delay_us(6);
	USISR |= (1<<USISIF);
	USICR |= 0x01;
	_delay_us(3);
}

void I2CStop(void)
{
	USICR |= 0x01;
	_delay_us(5);
	USICR &= ~(1<<USICS0);
	USIDR = 0x80;
	USISR |= (1<<USIPF);
	_delay_us(5);
}

void I2CSendByte(uint8_t byte)
{
	USISR = 0x00;
	TCNT0 = 0x00;
	USIDR = byte;
	
	while((USISR & 0x09) != 0x09) {

		if((TIFR & (1 << OCF0B)) == (1 << OCF0B)){
			USICR |= (1 << USITC);
			_delay_us(CNT_TOP/2);
			USICR |= (1 << USITC);
			TIFR |= (1 << OCF0B);
		}
		
		if((USISR & 0x08) == 0x08){
			DDRB &= ~(1 << PB0);
			PORTB &= ~(1 << PB0);
		}
	}
	DDRB |= (1 << PB0);
	PORTB |= (1 << PB0);
	USIDR = 0x00;
}

uint8_t I2CReadByte(uint8_t ack)
{
	uint8_t byte=0, lock=0;
	USISR = 0x00;
	TCNT0 = 0x00;
	
	DDRB &= ~(1 << PB0);
	PORTB &= ~(1 << PB0);
	
	while((USISR & 0x09) != 0x09) {

		if((TIFR & (1 << OCF0B)) == (1 << OCF0B)){
			USICR |= (1 << USITC);
			_delay_us(CNT_TOP/2+10);
			USICR |= (1 << USITC);
			TIFR |= (1 << OCF0B);
		}
		
		if((USISR & 0x08) == 0x08 && !lock){
			byte = USIDR;
			lock=1;
			
			DDRB |= (1 << PB0);
			if(!ack) PORTB &= ~(1 << PB0);
			else USIDR |= 0x80, PORTB |= (1 << PB0);
		}
	}
	
	if(ack) USISR |= (1<<USISIF);
	PORTB |= (1 << PB0);
	USIDR = 0x00;

	return byte;
}

// this interrupt makes MCU leave sleep mode
ISR(PCINT0_vect)
{
    GIMSK = 0x00;
    PCMSK = 0x00;
}
