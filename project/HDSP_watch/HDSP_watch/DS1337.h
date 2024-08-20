/*
 * DS1337.h
 *
 * Created: 01.08.2022 13:33:31
 *  Author: drspa
 */ 


#ifndef DS1337_H_
#define DS1337_H_

#include "usi_driver.h"
#include <string.h>

uint8_t DS1337GetBCD(uint8_t num);
void DS1337Init(void);
void DS1337GetTime(char *time);
void DS1337GetDate(char *time);
void DS1337SetTime(uint8_t hor,
                   uint8_t min,
                   uint8_t sec,
                   uint8_t day,
                   uint8_t mon,
                   uint8_t dat,
                   uint8_t yer);


#endif /* DS1337_H_ */