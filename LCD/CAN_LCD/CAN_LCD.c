
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
#include "SPLC501C.h"
#include "timer.h"
#include "ADC.h"
#include "config.h"
#include "can_lib.h"
#include "graphic.h"
#include "SPLC501C.h"
#include "button.h"









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
	uint16_t Sync_Timer=0;
	uint8_t Receive_Switch=0;
	st_cmd_t rx_msg;
	uint8_t rx_buffer[8]={0};

	int16_t voltage=0;
	int16_t current=0;
	uint8_t SOC=0;
	uint8_t SOC_prev=0;
	uint8_t state=0;
	uint16_t V_low=0;
	uint16_t V_high=0;
	int8_t T_low=0;
	int8_t supply_voltage=0;
	int8_t winch_current=0;
	int8_t water=0;
	int8_t T_high=0;
	uint16_t temp=0;
	
	//wy³aczenie JTAG
    MCUCR |= (1<<JTD);
    MCUCR |= (1<<JTD);
	
	DDR(LED_PORT) |= (1<<LED_1);
	DDR(LED_H_PORT) |= (1<<LED_H);
	DDR(LED_L_PORT) |= (1<<LED_L);
	
	LED_H_ON;
	LED_L_ON;
	
	Init_Buttons();
	
	_delay_ms(5);
	
	GLCD_Initialize();
	_delay_ms(5);
    GLCD_ClearScreen();
    GLCD_GoTo(0,0);
	can_init(0);
	init_rtc();
	
	LED_1_OFF;
	
	sei();
	
	GLCD_Rectangle(102,12,30,51);
	GLCD_Rectangle_Full(111,9,12,3);
	
	unsigned char cnt = 0;
	
	while(1){
		if(timer_new_tick()){			
			
			Sync_Timer=timer_get();
			Send_Sync();
			
			if( !led_state ) {
				LED_1_ON;
				led_state = 1;
				} else {
				LED_1_OFF;
				led_state = 0;
			}
			
			//0x186
			if(Receive_Switch == 0){
				Receive_Msg(rx_buffer, 0x186, &rx_msg);
				while(timer_time_elapsed(Sync_Timer)<60){
					if(can_get_status(&rx_msg) == CAN_STATUS_COMPLETED){
						current = rx_buffer[0] + (rx_buffer[1]<<8);
						voltage = rx_buffer[2] + (rx_buffer[3]<<8);
						SOC = rx_buffer[4];
						state = rx_buffer[7];
						break;
					}
				}
				if(can_get_status(&rx_msg) != CAN_STATUS_COMPLETED){
					rx_msg.cmd = CMD_ABORT;
					while (can_cmd(&rx_msg) != CAN_CMD_ACCEPTED);
				}
				Receive_Switch++;
			}
			//0x206
			else if (Receive_Switch == 1){
				Receive_Msg(rx_buffer, 0x206, &rx_msg);
				while(timer_time_elapsed(Sync_Timer)<60){
					if(can_get_status(&rx_msg) == CAN_STATUS_COMPLETED){
						V_low = rx_buffer[0] + (rx_buffer[1]<<8);
						V_high = rx_buffer[3] + (rx_buffer[4]<<8);
						break;
					}
				}
				if(can_get_status(&rx_msg) != CAN_STATUS_COMPLETED){
					rx_msg.cmd = CMD_ABORT;
					while (can_cmd(&rx_msg) != CAN_CMD_ACCEPTED);
				}
				Receive_Switch++;
			}
			//0x190
			else if (Receive_Switch == 2){
				Receive_Msg(rx_buffer, 0x190, &rx_msg);
				while(timer_time_elapsed(Sync_Timer)<60){
					if(can_get_status(&rx_msg) == CAN_STATUS_COMPLETED){
						water = rx_buffer[1];
						winch_current = rx_buffer[3];
						supply_voltage = rx_buffer[2];
						break;
					}
				}
				if(can_get_status(&rx_msg) != CAN_STATUS_COMPLETED){
					rx_msg.cmd = CMD_ABORT;
					while (can_cmd(&rx_msg) != CAN_CMD_ACCEPTED);
				}
				//Receive_Switch++;
				Receive_Switch = 0;
			
			/*else if (Receive_Switch == 2){//0x286
				Receive_Msg(rx_buffer, 0x286, &rx_msg);
				while(Time_elapsed(Sync_Timer)<60){
					if(can_get_status(&rx_msg) == CAN_STATUS_COMPLETED){
						T_low = rx_buffer[0];
						T_high = rx_buffer[2];
						break;
					}
				}
				if(can_get_status(&rx_msg) != CAN_STATUS_COMPLETED){
					rx_msg.cmd = CMD_ABORT;
					while (can_cmd(&rx_msg) != CAN_CMD_ACCEPTED);
				}
				Receive_Switch = 0;
			}
			*/
					
			GLCD_GoTo(0,0);
			GLCD_WriteString_progmem(PSTR("State: "));
			GLCD_WriteString_progmem(PSTR("              "));
			GLCD_GoTo(36,0);
			switch(state){
				case 1 :
					GLCD_WriteString_progmem(PSTR("Init"));
					break;
				case 2 :
					GLCD_WriteString_progmem(PSTR("Discharging"));
					break;
				case 3 :
					GLCD_WriteString_progmem(PSTR("Battery Low"));
					break;
				case 4 :
					GLCD_WriteString_progmem(PSTR("Battery Empty"));
					break;
				case 5 :
					GLCD_WriteString_progmem(PSTR("Charging"));
					break;
				case 6 :
					GLCD_WriteString_progmem(PSTR("Battery High"));
					break;
				case 7 :
					GLCD_WriteString_progmem(PSTR("Battery Full"));
					break;
				case 15 :
					GLCD_WriteString_progmem(PSTR("Ready"));
					break;
				case 16 :
					GLCD_WriteString_progmem(PSTR("Error"));
					break;
				case 32 :
					GLCD_WriteString_progmem(PSTR("Second Prot."));
					break;
				case 48 :
					GLCD_WriteString_progmem(PSTR("Over Current"));
					break;
				case 64 :
					GLCD_WriteString_progmem(PSTR("Over Temp."));
					break;
				case 80 :
					GLCD_WriteString_progmem(PSTR("LEM Alarm"));
					break;
				case 96 :
					GLCD_WriteString_progmem(PSTR("Invalid Input"));
					break;
				case 224 :
					GLCD_WriteString_progmem(PSTR("Power Save"));
					break;
				case 240 :
					GLCD_WriteString_progmem(PSTR("Parametrization"));
					break;
	
				default :
					GLCD_WriteString_progmem(PSTR("Invalid"));
					break; 
			}
			
			GLCD_GoTo(0,1);
			GLCD_WriteString_progmem(PSTR("Voltage:"));
			//GLCD_clear(5);
			GLCD_voltage((((uint32_t)voltage)*100)/16);
			GLCD_WriteString_progmem(PSTR("V"));
			
			GLCD_GoTo(0,2);
			GLCD_WriteString_progmem(PSTR("Current:"));
			//GLCD_clear(5);
			GLCD_voltage((((uint32_t)current)*100)/16);
			GLCD_WriteString_progmem(PSTR("A"));
			
			/*GLCD_GoTo(0,3);
			GLCD_WriteString_progmem(PSTR("SOC: "));
			GLCD_clear(4);
			GLCD_dec(SOC);
			GLCD_WriteString_progmem(PSTR("%"));
			*/
			
			GLCD_GoTo(0,3);
			GLCD_WriteString_progmem(PSTR("Vmin:"));
			//GLCD_clear(6);
			GLCD_dec(V_low);
			GLCD_WriteString_progmem(PSTR("mV"));
			
			GLCD_GoTo(0,4);
			GLCD_WriteString_progmem(PSTR("Vmax:"));
			//GLCD_clear(6);
			GLCD_dec(V_high);
			GLCD_WriteString_progmem(PSTR("mV"));
			
			GLCD_GoTo(0,5);
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
			
			if( button_state == 1 ) {
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
			}
			
			
		}
	}

}
}

void Send_Sync(void){

	
	U8 tx_buffer[8]={0};
    st_cmd_t tx_msg;
	
	// --- Init variables
    tx_msg.pt_data = &tx_buffer[0];
    tx_msg.status = 0;
	tx_msg.id.std = 0x80;
	tx_msg.ctrl.ide = 0;
	tx_msg.ctrl.rtr = 0;
	tx_msg.dlc = 0;
	tx_msg.cmd = CMD_TX_DATA;
	// --- Tx Command
	while(can_cmd(&tx_msg) != CAN_CMD_ACCEPTED);

	// --- Wait for Tx remote completed
	while(can_get_status(&tx_msg) == CAN_STATUS_NOT_COMPLETED);
}

void Send_Abort(void){

	
	U8 tx_buffer[8]={0};
    st_cmd_t tx_msg;
	
	// --- Init variables
    tx_msg.pt_data = &tx_buffer[0];
    tx_msg.status = 0;
	tx_msg.id.std = 0x186;
	tx_msg.ctrl.ide = 0;
	tx_msg.ctrl.rtr = 0;
	tx_msg.dlc = 8;
	tx_msg.cmd = CMD_ABORT;
	// --- Tx Command
	while(can_cmd(&tx_msg) != CAN_CMD_ACCEPTED);

	// --- Wait for Tx remote completed
	while(can_get_status(&tx_msg) == CAN_STATUS_NOT_COMPLETED);
}

void Receive_Msg(uint8_t *buf, uint16_t id, st_cmd_t *rx_msg){

	uint8_t i;
	
    
	for(i=0; i<8; i++) buf[i]=0;
	
	// --- Init variables
    (*rx_msg).pt_data = buf;
    (*rx_msg).status = 0;
	(*rx_msg).id.std = id;
	(*rx_msg).ctrl.ide = 0;
	(*rx_msg).ctrl.rtr = 0;
	(*rx_msg).dlc = 8;
	(*rx_msg).cmd = CMD_RX_DATA_MASKED;
	// --- Tx Command
	while(can_cmd(rx_msg) != CAN_CMD_ACCEPTED);

	// --- Wait for Tx remote completed
	//while(can_get_status(&rx_msg) == CAN_STATUS_NOT_COMPLETED);
}


	
//-----------------------------------------------------
