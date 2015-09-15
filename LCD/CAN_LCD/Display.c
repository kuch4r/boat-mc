/*
 * Display.c
 *
 * Created: 2015-09-13 19:29:31
 *  Author: user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "CAN.h"
#include "harddef.h"
#include "graphic.h"
#include "SPLC501C.h"
#include "harddef.h"
#include "makra.h"
#include "button.h"
#include "display.h"

extern struct CAN_str CAN;


int8_t winch_current=0;
int8_t water=0;



void Display_Init(){
	
	//Inicjalizacja portów
	DDR(LCD_RESET_PORT) |= (1<<LCD_RESET);
	
	//sprzêtowy reset
	LCD_RESET_ON;
	_delay_ms(5);
	LCD_RESET_OFF;
	
	//inicjalizacja softwarowa
	GLCD_Initialize();
	_delay_ms(5);
	GLCD_ClearScreen();
	GLCD_GoTo(0,0);
	
	//ramka baterii - do przeniesienia gdzie indziej
	GLCD_Rectangle(102,12,30,51);
	GLCD_Rectangle_Full(111,9,12,3);

};

void Display_Task(){
	static uint8_t line_shift=0;
	static uint8_t line_refresh_nr=0;
	static uint8_t SOC_prev=0;
	static uint16_t temp=0;
	static uint8_t line_shift_prev=0;
	static uint8_t line_clear_iteration=0;
	
	line_shift=Buttons_get_screen_line();
	
	line_refresh_nr++;
	if(line_refresh_nr >= DISPLAY_LINES_COUNT ){
		line_refresh_nr = 0;
	}
	
	GLCD_GoTo(0,line_refresh_nr);
	if(line_shift != line_shift_prev){
		GLCD_clear(16);
		line_clear_iteration++;
		if(line_clear_iteration >= DISPLAY_LINES_COUNT){
			line_shift_prev = line_shift;
			line_clear_iteration=0;
		}
	}
	
	if(line_refresh_nr == (0 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("State:"));
		GLCD_clear(10);
		switch(CAN.BMS_state){
			case 1 :
			GLCD_WriteString_progmem(PSTR("Init"));
			break;
			case 2 :
			GLCD_WriteString_progmem(PSTR("Discharge"));
			break;
			case 3 :
			GLCD_WriteString_progmem(PSTR("Batt Low"));
			break;
			case 4 :
			GLCD_WriteString_progmem(PSTR("Batt Empty"));
			break;
			case 5 :
			GLCD_WriteString_progmem(PSTR("Charging"));
			break;
			case 6 :
			GLCD_WriteString_progmem(PSTR("Batt High"));
			break;
			case 7 :
			GLCD_WriteString_progmem(PSTR("Batt Full"));
			break;
			case 15 :
			GLCD_WriteString_progmem(PSTR("Ready"));
			break;
			case 16 :
			GLCD_WriteString_progmem(PSTR("Error"));
			break;
			case 32 :
			GLCD_WriteString_progmem(PSTR("Sec Prot"));
			break;
			case 48 :
			GLCD_WriteString_progmem(PSTR("Over Curr"));
			break;
			case 64 :
			GLCD_WriteString_progmem(PSTR("Over Temp"));
			break;
			case 80 :
			GLCD_WriteString_progmem(PSTR("LEM Alarm"));
			break;
			case 96 :
			GLCD_WriteString_progmem(PSTR("Inv Inp"));
			break;
			case 224 :
			GLCD_WriteString_progmem(PSTR("Pwr Save"));
			break;
			case 240 :
			GLCD_WriteString_progmem(PSTR("Param"));
			break;
			
			default :
			GLCD_WriteString_progmem(PSTR("Invalid"));
			break;
		}
	}
	
			
	else if(line_refresh_nr == (1 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Voltage:"));
		GLCD_voltage((((uint32_t)CAN.BMS_voltage)*100)/16);
		GLCD_WriteString_progmem(PSTR("V"));
	}
			
	else if(line_refresh_nr == (2 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Current:"));
		GLCD_voltage((((uint32_t)CAN.BMS_current)*100)/16);
		GLCD_WriteString_progmem(PSTR("A"));
	}
			
	else if(line_refresh_nr == (3 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("SOC:"));
		GLCD_clear(4);
		GLCD_dec(CAN.BMS_SOC);
		GLCD_WriteString_progmem(PSTR("%"));
	}
	
	else if(line_refresh_nr == (4 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Vmin:"));
		GLCD_dec(CAN.BMS_V_min);
		GLCD_WriteString_progmem(PSTR("mV @ "));
		GLCD_clear(2);
		GLCD_dec(CAN.BMS_V_min_nr);
	}
			
	else if(line_refresh_nr == (5 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Vmax:"));
		GLCD_dec(CAN.BMS_V_max);
		GLCD_WriteString_progmem(PSTR("mV @ "));
		GLCD_clear(2);
		GLCD_dec(CAN.BMS_V_max_nr);
	}
	
	else if(line_refresh_nr == (6 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Vavrg:"));
		GLCD_dec(CAN.BMS_V_avrg);
		GLCD_WriteString_progmem(PSTR("mV"));
	}
	
	else if(line_refresh_nr == (7 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Tmin:"));
		GLCD_clear(6);
		GLCD_dec(CAN.BMS_T_min);
		GLCD_WriteString_progmem(PSTR("stC"));
	}
	
	else if(line_refresh_nr == (8 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("Tmax:"));
		GLCD_clear(6);
		GLCD_dec(CAN.BMS_T_max);
		GLCD_WriteString_progmem(PSTR("stC"));
	}
	
	else if(line_refresh_nr == (9 + line_shift) % SCREEN_LINES_COUNT){
		GLCD_WriteString_progmem(PSTR("DMCC:"));
		GLCD_clear(6);
		GLCD_dec(CAN.BMC_DMCC_p);
		GLCD_WriteString_progmem(PSTR("%"));
	}
	
	if(line_refresh_nr == 0 ){
		
		if(CAN.BMS_SOC != SOC_prev){
			if(CAN.BMS_SOC > SOC_prev){
				
				if(CAN.BMS_SOC > 100){
					 CAN.BMS_SOC=100;
				}
				temp = CAN.BMS_SOC/2;
				GLCD_Rectangle_Full(103,62-temp,28,temp);
			}
			else {
				temp = 50-CAN.BMS_SOC/2;
				if(temp>49) temp=49;
				GLCD_Rectangle_Clear(103,13,28,temp);
			}
			SOC_prev=CAN.BMS_SOC;
		}
	}
	/*GLCD_GoTo(0,5);
	GLCD_WriteString_progmem(PSTR("Water:"));
	//GLCD_clear(6);
	GLCD_dec(water);
	GLCD_WriteString_progmem(PSTR("L"));
			
	GLCD_GoTo(0,6);
	GLCD_WriteString_progmem(PSTR("12V Akku:"));
	//GLCD_clear(6);
	GLCD_voltage((((uint32_t)supply_voltage)*10));
	GLCD_WriteString_progmem(PSTR("V"));
			
	GLCD_GoTo(0,7);
	GLCD_WriteString_progmem(PSTR("W Current:"));
	//GLCD_clear(6);
	GLCD_voltage((((uint32_t)winch_current)*50));
	GLCD_WriteString_progmem(PSTR("A"));
			
	/*GLCD_GoTo(0,6);
	GLCD_WriteString_progmem(PSTR("Tmin: "));
	GLCD_clear(6);
	GLCD_dec(T_low);
	GLCD_WriteString_progmem(PSTR("stC"));
			
	GLCD_GoTo(0,7);
	GLCD_WriteString_progmem(PSTR("Tmax: "));
	GLCD_clear(6);
	GLCD_dec(T_high);
	GLCD_WriteString_progmem(PSTR("stC"));
	*/
			
	/*if( button_state == 1 ) {
		if( cnt == 14 ){
			GLCD_Rectangle_Clear(103,13,28,49);
			cnt = 0;
		}
		GLCD_Rectangle_Full(104, 58-(cnt*4),26,3);
		cnt++;
		if( cnt >= 13 ){
			cnt = 0;
			GLCD_Rectangle_Clear(103,13,28,49);
		}
		SOC_prev = 127;
	} else {
		if( SOC_prev == 127) {
			GLCD_Rectangle_Clear(103,13,28,49);
			SOC_prev = 0;
			cnt = 14;
		}
		if(SOC!=SOC_prev){
			if(SOC>SOC_prev){
						
				if(SOC>100) SOC=100;
				temp = SOC/2;
				GLCD_Rectangle_Full(103,62-temp,28,temp);
			}
			else {
				temp = 50-SOC/2;
				if(temp>49) temp=49;
				GLCD_Rectangle_Clear(103,13,28,temp);
			}
			SOC_prev=SOC;
		}
		button_state = 0;
	}*/
}

