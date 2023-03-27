/*
 * HDSP_watch.c
 *
 * Created: 01.04.2022 19:29:26
 * Author : Dr. Space
 */ 
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include "DS1337.h"
#include "HDSP_2000.h"

// this interrupt makes MCU leave sleep mode
ISR(PCINT0_vect)
{
	//MCUCR &= ~(1<<SE);
	MCUCR = 0x10;
}

// Port initialization
void port_init(void)
{
	CLKPR = 0x80;
	CLKPR = 0x00;
	DDRB = 0x0F;
}

void pc_int_init(void)
{
	GIMSK = 0x20; // enable external interrupt INT0 
	PCMSK = 0x10; // enable interrup from pin PC4
	sei(); // enable all interrupts
}

// disables all interrupts
void pc_int_stop(void)
{
	GIMSK = 0x00;
	PCMSK = 0x00;
	cli();
}

void ADC_init(void)
{
	ADMUX = 0x02; // select pin PC4 as analog input
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // ADC clock is f_cpu/128
}

int ADC_convert(void)
{
	int result, res_h;
	
	ADCSRA |= (1<<ADEN); // enable ADC
	ADCSRA |= (1<<ADSC); // start conversion
	
	// wait until conversion is done
	while((ADCSRA & (1<<ADIF)) != (1<<ADIF)) {}
	
	ADCSRA |= (1<<ADIF); // clear interrupt flag
	ADCSRA &= ~(1<<ADEN); // disable ADC
	
	result = ADCL;
	res_h = ADCH;
	result |= (res_h<<8);
	
	return result;
}

uint8_t get_BCD(uint8_t num)
{
	uint8_t tmp;
	if(num > 9){
		tmp = num;
		num %= 10;
		tmp /= 10;
		num |= (tmp<<4);
	}
	return num;
}

int main(void)
{
	char text[55], *tim;
	uint8_t ptr = 0, ptr2, *ptr3;
	uint8_t ln, n=0;
	uint8_t charge;
	uint8_t hor=0, min=0, sec=0, day=0, mon=0, dat=0, yer=0;
	uint8_t max_val[7] = {99, 30, 11, 6, 59, 59, 23};
	int adc_input;
	
	port_init();
	PWM_init();
	ADC_init();
	I2C_init();
	DS1307_init();
	pc_int_init();
	MCUCR |= (1<<SM1); // select "power-down" sleep mode
	
	while (1)
    {	
		MCUCR |= (1<<SE); // enable sleep mode
		
		__asm__ __volatile__ ( "sleep" "\n\t" :: );
		
		pc_int_stop(); // diable interuupts
		memset(text, ' ', 55); // preparing string that will show time
		
		// read the voltage value of battery
		PORTB |= (1<<PB3);
		adc_input = ADC_convert();
		adc_input = ((adc_input - 491) / 5) * 4;
		PORTB &= ~(1<<PB3);
		charge = adc_input;
		
		// in each iteration ptr varible increments by 1 with delay of 125 ms.
		// It means that each time text with time will shift by one digit to
		// the left. When ptr = 9 text will stop shifting for 124*24 ms. After
		// that text will continue to shift until it will disappear on display
		while(ptr < 16){
			tim = malloc(sizeof(char)*8);
			DS1307_get_Time(tim);
			strncpy(text+8, tim, 8);
			SPI_init();
			_delay_us(80);
			send_text(text+ptr, 125);
			ptr++;
		
			if(ptr == 9 && n<24) ptr=8, n++;
			else n=0;

			free(tim);
			I2C_init();
			if(n==8) adc_input = ADC_convert();
		}
		
		// here we check if button 2 was pressed
		// which means that watch enters setting mode.
		// Otherwise watch just display date
		ptr = 0;
		if(!(adc_input<100)) goto skip; 
		ln = 1;
		
		ptr2 = 16+ln+4+(charge/100);
		n = charge/100;
		
		adc_input = charge;
		
		// this loop sends date (day, month, year and charge) to display
		while(ptr < ptr2) {
			charge = adc_input;
			tim = malloc(sizeof(char)*30);
			DS1307_get_Date(tim);
			ln = strlen(tim);
			ptr2 = 21+ln+(n);
			strncpy(text+8, tim, ln);
			strncpy(text+8+ln, ", Charge: ", 10);
			text[ptr2-1] = '%';
			text[ptr2-2] = 0x30 | (charge % 10);
			charge /= 10;
			text[ptr2-3] = 0x30 | (charge % 10);
			charge /= 10;
			text[ptr2-4] = (0x20 + (n<<4)) | charge;
			SPI_init();
			send_text(text+ptr, 200);
			ptr++;
			free(tim);
			I2C_init();
		}
		n=0;
		ptr = 0;
		goto skip2;
skip:		// here starts settings mode
		if(!((adc_input>100)&&(adc_input<900))) goto skip2;

		n=7;
		
		while(n>0) {
			
			strcpy_P(text, (char *)pgm_read_word(&(set_table[n-1])));
			SPI_init();
			scrool_text2(text, 80);
loop:
			switch(n) {
				case 7:
				ptr3 = &hor;
				text[6] = 0x30 | (get_BCD(hor) >> 4);
				text[7] = 0x30 | (get_BCD(hor) & 0x0F);
				break;
				case 6:
				ptr3 = &min;
				text[6] = 0x30 | (get_BCD(min) >> 4);
				text[7] = 0x30 | (get_BCD(min) & 0x0F);
				break;
				case 5:
				ptr3 = &sec;
				text[6] = 0x30 | (get_BCD(sec) >> 4);
				text[7] = 0x30 | (get_BCD(sec) & 0x0F);
				break;
				case 4:
				ptr3 = &day;
				strcpy_P(text+5, (char *)pgm_read_word(&(days_table[day])));
				break;
				case 3:
				ptr3 = &mon;
				strcpy_P(text+5, (char *)pgm_read_word(&(monthes_table[mon])));
				break;
				case 2:
				ptr3 = &dat;
				text[6] = 0x30 | (get_BCD(dat+1) >> 4);
				text[7] = 0x30 | (get_BCD(dat+1) & 0x0F);
				break;
				case 1:
				ptr3 = &yer;
				text[6] = 0x30 | (get_BCD(yer) >> 4);
				text[7] = 0x30 | (get_BCD(yer) & 0x0F);
				break;
				default:
				ptr3 = &hor;
				break;
			}
			
			SPI_init();
			send_text(text, 150);
			I2C_init();
			
			adc_input = ADC_convert();
			
			if(adc_input<100) (*ptr3)++;
			
			if(*ptr3 > max_val[n-1]) *ptr3 = 0;
			
			if(!((adc_input>100)&&(adc_input<900))) goto loop;
			
			n--;
			SPI_init();
			scrool_text(text, 80);
			I2C_init();
		}
		
		I2C_start();
		//I2C_rep_start();
		I2C_send_byte(w_addr);
		I2C_send_byte(0x00);
		I2C_send_byte(get_BCD(sec));
		I2C_send_byte(get_BCD(min));
		I2C_send_byte(get_BCD(hor));
		I2C_send_byte(day+1);
		I2C_send_byte(get_BCD(dat+1));
		I2C_send_byte(get_BCD(mon+1));
		I2C_send_byte(get_BCD(yer));
		I2C_stop();
		
skip2:
		pc_int_init();
    }
}
