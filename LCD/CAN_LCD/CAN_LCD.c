
// Last modified: 26 October 2006
// Changed to work with WinAVR 20060421

// Created : 25 November 2005
// Compiled on WinAVR 20040404

// Includes
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>			// included to enable the writing of strings from rom
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "harddef.h"
#include "delay.h"
#include "makra.h"
#include "timer.h"
#include "ADC.h"
#include "config.h"
#include "can_lib.h"
#include "button.h"
#include "CAN.h"
#include "Display.h"


extern struct CAN_str CAN;







// included to enable the writing of strings from rom

// constants

//#define FCPU 6000000



//-----------------------------------------------------
// Main - a simple test program


SIGNAL(SIG_OUTPUT_COMPARE2);

void Send_Sync(void);
void Receive_Msg(uint8_t *buf, uint16_t id, st_cmd_t *rx_msg);
void Send_Abort(void);

int8_t led_state = 0; 

int main( void )
{
	uint16_t display_timer=0;
	//wy³aczenie JTAG
    MCUCR |= (1<<JTD);
    MCUCR |= (1<<JTD);
	
	DDR(LED_PORT) |= (1<<LED_1);
	DDR(LED_H_PORT) |= (1<<LED_H);
	
	
	LED_H_ON;
		
	sei();
		
	Buttons_Init();
	Display_Init();
	CAN_Init();
	Timer0_Init();
	
	LED_1_OFF;
	
	
	
	display_timer = timer_get();
	
	while(1){
		if(timer_new_tick()){		

			if( !led_state ) {
				//LED_1_ON;
				led_state = 1;
				} else {
				//LED_1_OFF;
				led_state = 0;
			}
			CAN_Task();
			
			if(timer_time_elapsed(display_timer) > DISPLAY_REFRESH_PERIOD){
				Display_Task();
				display_timer = timer_get();
			}
			
		}
	}
	
}





	
//-----------------------------------------------------
