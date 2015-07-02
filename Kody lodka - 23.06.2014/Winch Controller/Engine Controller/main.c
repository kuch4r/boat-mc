/*
 * BLDC.c
 *
 * Created: 2013-02-03 11:12:14
 *  Author: LukaszKucharski
 
 CKSEL 0001
 */ 


#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "CAN.h"
#include "main.h"
#include "harddef.h"
#include "makra.h"
#include "timer.h"
#include "ADC.h"
#include "PSC.h"


extern struct CAN_str CAN;
extern struct ADC_str ADC_RAW;
extern struct PSC_str PSC;

volatile int8_t winch_state=OFF;
volatile uint8_t power_up_source=SWITCH_ON;
volatile uint8_t winch_status=INIT;
volatile uint16_t winch_timer=0;

int main(void) {
	uint16_t life_timer = 0;
	uint16_t time_stamp = 0;
	
	uint16_t current = 0;
	uint16_t water = 0;
	uint16_t supply_voltage = 0;
	
	Init_HAL();
	Init_Timer();
	Init_CAN(Get_DIP_sw_word());
	Init_ADC();
	Init_PSC();
	sei();
	
	
	if( WINCH_UP_SET && WINCH_DOWN_SET ){
		power_up_source = SWITCH_ON;
	}
	else if(WINCH_UP_SET){
		PWR_ON;
		power_up_source = WINCH_ON;
		winch_state = UP;
	}
	else if(WINCH_DOWN_SET){
		PWR_ON;
		power_up_source = WINCH_ON;
		winch_state = DOWN;
	}
	else{
		power_up_source = SWITCH_ON;
	}
	
	PWR_ON;
	
	ADC_force_conversion();
	
	time_stamp=Get_timer();
	life_timer=Get_timer();
	
    while(1)
    {
		if(Timer_new_tick()){
			
			CAN_Task();
			ADC_task();
			PSC_Task();
			
			CAN.state = winch_status;
			CAN.diag_1 = WINCH_SET;
			CAN.current = current;
			CAN.water = water;
			CAN.supply_voltage = supply_voltage;
			
			//gdy brak aktywnoœci przez okreœlony czas - wy³¹cz siê 
			if(Time_elapsed(life_timer) > TIME_TO_LIVE){
				PWR_OFF;
				_delay_ms(200);
				life_timer=Get_timer();
				PWR_ON;
			}
			
			//wskazywanie stanu wyci¹garki
			if(winch_state == UP || winch_state == DOWN){
				LED_0_ON;
				if( BOARD_IS_ACTIVE ) {
					LED_1_ON;
				} else {
					LED_1_OFF;
				}
			}
			else if(winch_state == OVERLOAD){
				LED_0_OFF;
				LED_1_ON;
			}
			else{
				LED_0_OFF;
				LED_1_OFF;
			}
			
			//przeliczanie napiêcia
			if((ADC_RAW.supply_voltage_raw + V_OFFSET) < 1024 && (V_OFFSET+ADC_RAW.supply_voltage_raw) > 0){
				supply_voltage=((((uint32_t)ADC_RAW.supply_voltage_raw + V_OFFSET))*V_SCALE) / 1024; //przeliczanie ADU na pr¹d (ADU*I_SCALE)/1024 i wyliczanie œredniej z poprzednim pomiarem
			}
			else{
				supply_voltage=0;
			}
			
			//przeliczanie wody
			if((ADC_RAW.water_raw + W_OFFSET) < 1024 && (W_OFFSET + ADC_RAW.water_raw) > 0){
				water=((((uint32_t)ADC_RAW.water_raw + W_OFFSET))*W_SCALE) / 1024; //przeliczanie ADU na pr¹d (ADU*I_SCALE)/1024 i wyliczanie œredniej z poprzednim pomiarem
			} else {
				water = 0;
			}
		
			//przeliczanie pr¹du
			if(((ADC_RAW.current_raw + I_OFFSET) < 1024) && ((I_OFFSET + ADC_RAW.current_raw) > 0)){
				current=((((uint32_t)ADC_RAW.current_raw + I_OFFSET))*I_SCALE) / 1024; //przeliczanie ADU na pr¹d (ADU*I_SCALE)/1024 i wyliczanie œredniej z poprzednim pomiarem
			} else {
				current = 0;
			}
			
			if(winch_state == OVERLOAD){
				//BUZZER_ON;
				
			}
			else if(winch_state == OFF){
				//BUZZER_OFF;
			}
			//gdy wyci¹garka aktywna
			else if(winch_state == UP || winch_state == DOWN){
				
				if(((WINCH_IS_ACTIVE && current >= MAX_CURRENT_WINCH) || (BOARD_IS_ACTIVE && current >= MAX_CURRENT_BOARD)) && Time_elapsed(winch_timer) >= CURRENT_BLIND_TIME){ //zabezpiecznie nadpr¹dowe
					winch_state=OVERLOAD;
					Disable_PSC();
					
				}
				
				life_timer = Get_timer();//pobijanie timer-a od samowy³¹czenia
				
				if(winch_status == INIT){
					time_stamp = Get_timer();
					winch_timer=Get_timer();
					winch_status = INIT_DELAY;
				}
				else if( winch_status == INIT_DELAY){ //poczatkowe opóŸnienie a¿ siê zamkn¹ styczniki
					if(Time_elapsed(time_stamp) >= INITIAL_DELAY){
						PSC.torque = GET_TORQUE_INIT_VAL;
						winch_status = TORQUE_INCREASING;
						time_stamp = Get_timer();
						winch_timer = Get_timer();
						Enable_PSC();
					}
				}
				else if( winch_status == TORQUE_INCREASING){
					if(Time_elapsed(time_stamp) >= GET_TORQUE_RISING_SPEED_PERIOD){
						time_stamp=Get_timer();
						if(PSC.torque <= (1000-GET_TORQUE_RISING_SPEED)){
							PSC.torque += GET_TORQUE_RISING_SPEED;
						}
						else{
							PSC.torque = 1000; //FULL POWER
							winch_status = FULL_POWER;
						}	
					}					
				}
				else if(winch_status == FULL_POWER){//pe³na moc
					
				}
			}
		}
	}
};

void LED_blinking(uint8_t led_0, uint8_t led_1){
	static uint16_t led_time_stamp=0;
	static uint8_t led_status=0;
	
		if(Time_elapsed(led_time_stamp)>LED_BLINKING_PERIOD){
			
			led_time_stamp = Get_timer();
			
			if(led_status == 0){
				if(led_0)
				LED_0_ON;
				if(led_1)
				LED_1_ON;
				led_status = 1;
			}
			else{
				if(led_0)
				LED_0_OFF;
				if(led_1)
				LED_1_OFF;
				led_status = 0;
			}
		}
};

void LEDs_OFF(void){
	LED_0_OFF;
	LED_1_OFF;
	
};

void Init_HAL(void){
	
	//w³¹czenie PLL na 64MHz
	PLLCSR |= (1<PLLF) | (1<<PLLE);
	
	//ustawienie porty na wyjœciowe dla LED-ow
	DDR(LED_0_PORT) |= (1<<LED_0);
	DDR(LED_1_PORT) |= (1<<LED_1);
	
	LED_0_ON;
	LED_1_ON;
	
	//podtrzymanie zasilania
	DDR(PWR_PORT) |= (1<<PWR);
	
	//Inicjalizacja DIP Switchy
	DDR(SW_1_PORT) &= ~(1<<SW_1);
	DDR(SW_2_PORT) &= ~(1<<SW_2);
	DDR(SW_3_PORT) &= ~(1<<SW_3);
	DDR(SW_4_PORT) &= ~(1<<SW_4);
	DDR(SW_5_PORT) &= ~(1<<SW_5);
	DDR(SW_6_PORT) &= ~(1<<SW_6);
	DDR(SW_7_PORT) &= ~(1<<SW_7);
	DDR(SW_8_PORT) &= ~(1<<SW_8);
	
	//w³aczenie pull-up-ów
	PORT(SW_1_PORT) |= 1<<SW_1;
	PORT(SW_2_PORT) |= 1<<SW_2;
	PORT(SW_3_PORT) |= 1<<SW_3;
	PORT(SW_4_PORT) |= 1<<SW_4;
	PORT(SW_5_PORT) |= 1<<SW_5;
	PORT(SW_6_PORT) |= 1<<SW_6;
	PORT(SW_7_PORT) |= 1<<SW_7;
	PORT(SW_8_PORT) |= 1<<SW_8;
	
	//Inicjalizacja wejœæ UP i DOWN
	DDR(WINCH_UP_PORT) &= ~(1<<WINCH_UP);
	DDR(WINCH_DOWN_PORT) &= ~(1<<WINCH_DOWN);
	DDR(AIN_2_PORT) &= ~(1<<AIN_2);
	
	//aktywacja przerwañ
	PCICR |= (1<<PCIE0) | (1<<PCIE1);
	
	PCMSK0 |= (1<<PCINT2);
	PCMSK1 |= (1<<PCINT12);
	
};

uint8_t Get_DIP_sw_word(void){
	uint8_t result=0;
	if(SW_1_SET)
		result |= 1<<0;
	if(SW_2_SET)
		result |= 1<<1;
	if(SW_3_SET)
		result |= 1<<2;
	if(SW_4_SET)
		result |= 1<<3;
	if(SW_5_SET)
		result |= 1<<4;
	if(SW_6_SET)
		result |= 1<<5;
	if(SW_7_SET)
		result |= 1<<6;
	if(SW_8_SET)
		result |= 1<<7;
	return result; 
	
};

SIGNAL (PCINT0_vect){ //przerwanie od WINCH_UP
	if(WINCH_UP_SET && winch_state == OFF){
		winch_state = UP;
		PSC.torque = TORQUE_INIT_VAL;
		winch_timer=Get_timer();
		//Enable_PSC();
		if(power_up_source == SWITCH_ON){
			power_up_source = WINCH_ON;
			PWR_ON;
		}
	}
	else if(WINCH_UP_CLEAR && winch_state != OFF){
		winch_state = OFF;
		Disable_PSC();
		winch_status = INIT;
	}
};

SIGNAL (PCINT1_vect){ //przerwanie od WINCH_DOWN
	if(WINCH_DOWN_SET && winch_state == OFF){
		winch_state = DOWN;
		PSC.torque = TORQUE_INIT_VAL;
		winch_timer=Get_timer();
		//Enable_PSC();
		if(power_up_source == SWITCH_ON){
			power_up_source = WINCH_ON;
			PWR_ON;
		}
	}
	else if(WINCH_DOWN_CLEAR && winch_state != OFF){
		winch_state = OFF;
		Disable_PSC();
		winch_status = INIT;
	}
};
