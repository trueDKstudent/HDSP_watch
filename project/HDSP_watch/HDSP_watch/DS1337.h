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

void DS1307_get_Time(char *time);
void DS1307_get_Date(char *time);
void DS1307_init(void);


#endif /* DS1337_H_ */