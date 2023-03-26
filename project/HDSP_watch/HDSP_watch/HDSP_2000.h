/*
 * HDSP_2000.h
 *
 * Created: 01.08.2022 13:37:38
 *  Author: drspa
 */ 


#ifndef HDSP_2000_H_
#define HDSP_2000_H_

#include "usi_driver.h"

void send_text_proto(const char *text, float d, uint8_t dir, uint8_t shift);
void send_text(const char *text, float d);
void scrool_text(const char *text, float d);
void scrool_text2(const char *text, float d);


#endif /* HDSP-2000_H_ */