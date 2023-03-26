/*
 * DS1337.c
 *
 * Created: 01.08.2022 13:31:59
 *  Author: drspa
 */ 

#include "DS1337.h"

void DS1307_get_Time(char *time)
{
	uint8_t sec, min, hor;
	
	I2C_start();
	I2C_send_byte(w_addr);
	I2C_send_byte(0x00);
	_delay_us(3);
	I2C_rep_start();
	_delay_us(2);
	I2C_send_byte(r_addr);
	sec = I2C_read_byte(0);
	min = I2C_read_byte(0);
	hor = I2C_read_byte(1);
	_delay_us(5);
	I2C_stop();
	
	time[7] = 0x30 | (0x0F & sec);
	time[6] = 0x30 | ((0x70 & sec)>>4);
	time[5] = ':';
	time[4] = 0x30 | (0x0F & min);
	time[3] = 0x30 | ((0x70 & min)>>4);
	time[2] = ':';
	time[1] = 0x30 | (0x0F & hor);
	time[0] = 0x30 | ((0x30 & hor)>>4);
}

void DS1307_get_Date(char *time)
{
	uint8_t day, month, date, year;
	char buff[14];
	
	I2C_start();
	I2C_send_byte(w_addr);
	I2C_send_byte(0x03);
	_delay_us(3);
	I2C_rep_start();
	_delay_us(2);
	I2C_send_byte(r_addr);
	day = I2C_read_byte(0);
	date = I2C_read_byte(0);
	month = I2C_read_byte(0);
	year = I2C_read_byte(1);
	I2C_stop();
	
	strcpy_P(buff, (char *)pgm_read_word(&(days_table[day-1])));
	strcat(time, buff);
	
	buff[0] = 0x30 | (date>>4);
	buff[1] = 0x30 | (date&0x0F);
	buff[2] = ' ';
	buff[3] = 0;
	strcat(time, buff);
	
	month &= 0x7F;
	if(month >= 16) month -= 6;
	
	strcpy_P(buff, (char *)pgm_read_word(&(monthes_table[month-1])));
	strcat(time, buff);
	
	buff[0] = 0x30 | (year>>4);
	buff[1] = 0x30 | (year&0x0F);
	buff[2] = 0;
	
	strcat(time, buff);
}

void DS1307_init(void)
{
	I2C_start();
	I2C_send_byte(w_addr);
	I2C_send_byte(0x00);
	I2C_send_byte(0x00);
	I2C_send_byte(0x00);
	I2C_send_byte(0x00);
	I2C_stop();
}
