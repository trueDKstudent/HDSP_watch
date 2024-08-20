/*
 * ADC.h
 *
 * Created: 20.08.2024 11:29:17
 *  Author: drspa
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

typedef enum{
    BUT_SLEEP,
    BUT_SW_CHRG,
    BUT_SET_TIME
} Button_states;

void ADCInit(void);
int ADCConvert(void);
Button_states ADCGetButtonState(void);


#endif /* ADC_H_ */