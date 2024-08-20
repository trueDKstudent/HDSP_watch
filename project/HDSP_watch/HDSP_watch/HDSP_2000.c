/*
 * HDSP_2000.c
 *
 * Created: 01.08.2022 13:36:48
 *  Author: drspa
 */ 

#include "HDSP_2000.h"

/*
 *Function HDSPSendTextProto is a prototype function from which functions HDSPSendText,
 *HDSPScroolTextDown and HDSPScroolTextDown2 are made. Depending from the arguments function 
 *HDSPSendTextProto can display text in various ways. Argument shifts indicate by 
 *what amount of bits characters will be shifted up or down. Direction to which
 *characters will be shifted is set by argument dir. Argument d is a delay in ms
 *for which text will be displayed on screen(indicators). 
 */
static void HDSPSendTextProto(const char *text, float d, uint8_t dir, uint8_t shift)
{
	uint8_t i, j, cnt, tmp;
	
	while(d > 0){
		TCNT1 = 0x00;
		for(j=0; j<5; j++){
			for(i=8; i>0; i--){
				tmp = pgm_read_byte(&font[5*text[i-1]+j-160])<<1;
				if(dir) SPISend(tmp<<shift);
				else SPISend(tmp>>shift);
			}
			_delay_us(90);
			_delay_us(90);
			_delay_us(70);
		}
		cnt = TCNT1;
		d -= (float)(cnt*0.008);
	}
}

void HDSPSendText(const char *text, float d)
{
	HDSPSendTextProto(text, d, 1, 0);
}

void HDSPScroolTextDown(const char *text, float d)
{
	uint8_t ptr=1, N=8;
	
	do{
		HDSPSendTextProto(text, d, 1, ptr);
		N--;
		ptr++;
	} while (N>0);
}

void HDSPScroolTextDown2(const char *text, float d)
{
	uint8_t ptr=7, N=8;
	
	do{
		HDSPSendTextProto(text, d, 0, ptr);
		N--;
		ptr--;
	} while (N>0);
}
