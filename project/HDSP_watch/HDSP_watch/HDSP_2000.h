/*
 * HDSP_2000.h
 *
 * Created: 01.08.2022 13:37:38
 *  Author: drspa
 */ 


#ifndef HDSP_2000_H_
#define HDSP_2000_H_

#include "usi_driver.h"

void HDSPSendText(const char *text, float d);
void HDSPScroolTextDown(const char *text, float d);
void HDSPScroolTextDown2(const char *text, float d);


#endif /* HDSP-2000_H_ */