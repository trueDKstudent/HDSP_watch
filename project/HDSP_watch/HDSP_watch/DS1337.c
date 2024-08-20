/*
 * DS1337.c
 *
 * Created: 01.08.2022 13:31:59
 *  Author: drspa
 */ 

#include "DS1337.h"

uint8_t DS1337GetBCD(uint8_t num)
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

void DS1337GetTime(char *time)
{
	uint8_t sec, min, hor;
	
	I2CStart();
	I2CSendByte(WRITE_ADDR);
	I2CSendByte(0x00);
	_delay_us(3);
	I2CRepStart();
	_delay_us(2);
	I2CSendByte(READ_ADDR);
	sec = I2CReadByte(0);
	min = I2CReadByte(0);
	hor = I2CReadByte(1);
	_delay_us(5);
	I2CStop();
	
	time[7] = 0x30 | (0x0F & sec);
	time[6] = 0x30 | ((0x70 & sec)>>4);
	time[5] = ':';
	time[4] = 0x30 | (0x0F & min);
	time[3] = 0x30 | ((0x70 & min)>>4);
	time[2] = ':';
	time[1] = 0x30 | (0x0F & hor);
	time[0] = 0x30 | ((0x30 & hor)>>4);
}

void DS1337GetDate(char *time)
{
	uint8_t day, month, date, year;
	char buff[14];
	
	I2CStart();
	I2CSendByte(WRITE_ADDR);
	I2CSendByte(0x03);
	_delay_us(3);
	I2CRepStart();
	_delay_us(2);
	I2CSendByte(READ_ADDR);
	day = I2CReadByte(0);
	date = I2CReadByte(0);
	month = I2CReadByte(0);
	year = I2CReadByte(1);
	I2CStop();
	
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

void DS1337Init(void)
{
	I2CStart();
	I2CSendByte(WRITE_ADDR);
	I2CSendByte(0x00);
	I2CSendByte(0x00);
	I2CSendByte(0x00);
	I2CSendByte(0x00);
	I2CStop();
}


void DS1337SetTime(uint8_t hor, 
                     uint8_t min, 
                     uint8_t sec, 
                     uint8_t day, 
                     uint8_t mon, 
                     uint8_t dat, 
                     uint8_t yer)
{
    I2CStart();
    I2CSendByte(WRITE_ADDR);
    I2CSendByte(0x00);
    I2CSendByte(DS1337GetBCD(sec));
    I2CSendByte(DS1337GetBCD(min));
    I2CSendByte(DS1337GetBCD(hor));
    I2CSendByte(day+1);
    I2CSendByte(DS1337GetBCD(dat+1));
    I2CSendByte(DS1337GetBCD(mon+1));
    I2CSendByte(DS1337GetBCD(yer));
    I2CStop();
}