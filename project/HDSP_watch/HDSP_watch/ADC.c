/*
 * ADC.c
 *
 * Created: 20.08.2024 11:29:29
 *  Author: drspa
 */ 

#include "ADC.h"

void ADCInit(void)
{
    ADMUX |= (1 << MUX1); // select pin PC4 as analog input
    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // ADC clock is f_cpu/128
}

int ADCConvert(void)
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

Button_states ADCGetButtonState(void)
{
    Button_states state = BUT_SLEEP;
    int val1, val2;
    
    val1 = ADCConvert();
    val2 = ADCConvert();
    
    val1 = (val1 + val2) / 2;
    val2 = val1 - 856;
    
    if(val2 < 50 &&
       val2 > -50){
        state = BUT_SET_TIME;
    }else if(val2 < 50 &&
             val2 < -50){
        state = BUT_SW_CHRG;
    }else if(val2 > 50 &&
             val2 > -50){
        state = BUT_SLEEP;
    }
    
    return state;
}
