/*
 * HDSP_2000.c
 *
 * Created: 01.08.2022 13:36:48
 *  Author: drspa
 */ 

#include "HDSP_2000.h"

void send_text_proto(const char *text, float d, uint8_t dir, uint8_t shift)
{
	uint8_t i, j, cnt, tmp;
	
	while(d > 0){
		TCNT1 = 0x00;
		for(j=0; j<5; j++){
			for(i=8; i>0; i--){
				tmp = pgm_read_byte(&font[5*text[i-1]+j-160])<<1;
				if(dir) SPI_send(tmp<<shift);
				else SPI_send(tmp>>shift);
			}
			_delay_us(90);
			_delay_us(90);
			_delay_us(70);
		}
		cnt = TCNT1;
		d -= (float)(cnt*0.008);
	}
}

void send_text(const char *text, float d)
{
	send_text_proto(text, d, 1, 0);
}

void scrool_text(const char *text, float d)
{
	uint8_t ptr=1, N=8;
	
	do{
		send_text_proto(text, d, 1, ptr);
		N--;
		ptr++;
	} while (N>0);
}

void scrool_text2(const char *text, float d)
{
	uint8_t ptr=7, N=8;
	
	do{
		send_text_proto(text, d, 0, ptr);
		N--;
		ptr--;
	} while (N>0);
}

