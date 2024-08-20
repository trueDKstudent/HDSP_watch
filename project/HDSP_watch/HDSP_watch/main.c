/*
 * HDSP_watch.c
 *
 * Created: 01.04.2022 19:29:26
 * Author : Dr. Space
 */ 

#include <avr/sleep.h>
#include <stdlib.h>
#include "DS1337.h"
#include "HDSP_2000.h"
#include "ADC.h"

char text[55];

void TaskShowTime(void)
{
    char tim[8];
    uint8_t ptr = 0;
    uint8_t n=0;
    
    memset(text, ' ', 55); // preparing string that will show time
    
    // in each iteration ptr varible increments by 1 with delay of 125 ms.
    // It means that each time text with time will shift by one digit to
    // the left. When ptr = 9 text will stop shifting for 124*24 ms. After
    // that text will continue to shift until it will disappear on display
    while(ptr < 16){
        DS1337GetTime(tim);
        strncpy(text+8, tim, 8);
        SPIInit();
        _delay_us(80);
        HDSPSendText(text+ptr, 125);
        ptr++;
        
        if(ptr == 9 && n<24) ptr=8, n++;
        else n=0;

        I2CInit();
    }
}

void TaskShowDateAndCharge(void)
{
    char tim[30];
    uint8_t ptr = 0, ptr2;
    uint8_t ln = 1, n = 0;
    uint8_t charge;
    int adc_input;
    
    memset(text, ' ', 55);
    memset(tim, 0, 30);

    // read the voltage value of battery
    PORTB |= (1<<PB3);
    adc_input = ADCConvert();
    adc_input = ((adc_input - 491) / 5) * 4;
    PORTB &= ~(1<<PB3);
    charge = adc_input;

    n = charge/100;
    
    adc_input = charge;
    
    // this loop sends date (day, month, year and charge) to display.
    // The text scrols to the left.
    do{
        charge = adc_input;
        DS1337GetDate(tim);
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
        SPIInit();
        HDSPSendText(text+ptr, 200);
        ptr++;
        memset(tim, 0, 30);
        I2CInit();
    } while(ptr < ptr2);
}

void TaskSetTime(void)
{
    // max values for settings: max year, max date...
    // Remark: in RTC DS1337 monthes, dates and day of the week
    // start from value 0
    uint8_t hor=0, min=0, sec=0, day=0, mon=0, dat=0, yer=0;
    uint8_t max_val[7] = {99, 30, 11, 6, 59, 59, 23};
    uint8_t *ptr3;
    uint8_t n = 7; // the number of settings
    uint8_t button_state = BUT_SLEEP;                            

    while(n>0) {
    	
    	strcpy_P(text, (char *)pgm_read_word(&(set_table[n-1])));
    	SPIInit();
    	HDSPScroolTextDown2(text, 80); // scrool text down starting with empty screen

        do {
    	switch(n) {
    		case 7:
    		ptr3 = &hor;
    		text[6] = 0x30 | (DS1337GetBCD(hor) >> 4);
    		text[7] = 0x30 | (DS1337GetBCD(hor) & 0x0F);
    		break;
    		case 6:
    		ptr3 = &min;
    		text[6] = 0x30 | (DS1337GetBCD(min) >> 4);
    		text[7] = 0x30 | (DS1337GetBCD(min) & 0x0F);
    		break;
    		case 5:
    		ptr3 = &sec;
    		text[6] = 0x30 | (DS1337GetBCD(sec) >> 4);
    		text[7] = 0x30 | (DS1337GetBCD(sec) & 0x0F);
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
    		text[6] = 0x30 | (DS1337GetBCD(dat+1) >> 4);
    		text[7] = 0x30 | (DS1337GetBCD(dat+1) & 0x0F);
    		break;
    		case 1:
    		ptr3 = &yer;
    		text[6] = 0x30 | (DS1337GetBCD(yer) >> 4);
    		text[7] = 0x30 | (DS1337GetBCD(yer) & 0x0F);
    		break;
    		default:
    		ptr3 = &hor;
    		break;
    	}
    	
    	SPIInit();
    	HDSPSendText(text, 150);
    	I2CInit();
    	
        // here goes incrementation of one of the values that are being modified
    	button_state = ADCGetButtonState();
        if(button_state == BUT_SW_CHRG) (*ptr3)++;
        (*ptr3) %= (max_val[n-1] + 1);
        
        }while(button_state != BUT_SET_TIME);
        
    	n--;
    	SPIInit();
    	HDSPScroolTextDown(text, 80); // scrool text down
    	I2CInit();   
    }
    // write new date to RTC
    DS1337SetTime(hor, min, sec, day, mon, dat, yer);
}

int main(void)
{
    uint8_t button_state;
    
	PortInit();
	PWMInit();
	ADCInit();
	I2CInit();
	DS1337Init();
	PcIntInit();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	while (1)
    {	
		sleep_mode();
		PcIntDeinit();

		TaskShowTime();
		button_state = ADCGetButtonState();
        
        if(button_state == BUT_SW_CHRG){
            TaskShowDateAndCharge();
        }else if(button_state == BUT_SET_TIME){
            TaskSetTime();
        }else if(button_state == BUT_SLEEP){
            //do nothig
        }
        
		PcIntInit();
    }
}
