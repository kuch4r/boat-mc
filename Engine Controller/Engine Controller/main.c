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

extern struct CAN_str CAN;


int main(void)
{
	uint8_t state=INIT;
	uint8_t direction=N;
	uint8_t throttle_direction=N;
	uint16_t time_stamp=0;
	int16_t target_torque=0;
	uint16_t throttle_position=0;
	int16_t throttle_percentage=0;
	uint8_t init_counter=0;
	
	Init_HAL();
	Init_Timer();
	Init_CAN();
	Init_ADC();
	sei();
	
	//ustwienie wejœci i start konwersji
	ADC_switch(POT_IN_ADC);
	ADC_force_conversion();
	
	time_stamp=Get_timer();
	while(Time_elapsed(time_stamp)<1000);
	time_stamp=Get_timer();
	LED_blinking(1,1,1);
	
    while(1)
    {
		if(Timer_new_tick()){
			
			CAN_Task();
			ADC_task();
			
			CAN.state=state;
			
			if(ADC_conversion_completed()){
				throttle_position=ADC_get();
			}
			
			if(state == INIT){
				//oczekuj na falownik
								
				//warunek przejscia do nastepnego stanu
				if(CAN_Heart_Beat_received()){
					time_stamp=Get_timer();
					state = INV_INIT;
					Set_NMT_flag();
					LEDs_OFF();
				}
			}
			else if(state == INV_INIT){
				//inicjalizacja falownika
				LED_blinking(1,1,0);
				if((CAN.status_word & 0x0040) == 0x0040 && init_counter == 0){
					CAN.control_word = 0x0006;
					Clear_NMT_flag();
					init_counter++;
				}
				else if((CAN.status_word & 0x0021) == 0x0021 && init_counter == 1){
					CAN.control_word = 0x0007;
					init_counter++;
				}
				else if((CAN.status_word & 0x0023) == 0x0023 && init_counter == 2){
					CAN.control_word = 0x000f;
					init_counter++;
				}
				else if((CAN.status_word & 0x0027) == 0x0027 && init_counter == 3){//warunek przeœjcia do nastêpnego stanu
					time_stamp=Get_timer();
					state = WAITING_FOR_ZERO;
					LEDs_OFF();
				}
			}
			else if(state == WAITING_FOR_ZERO){
				//oczekuj na zero potencjometru
				//migaj zielon¹ diod¹
				LED_blinking(1,0,0);
				//jeœli w zakresie zera to idŸ dalej
				if((throttle_position >= (CENTER - BLIND_RANGE)) && (throttle_position <= (CENTER + BLIND_RANGE))){
					state = RUN;
					time_stamp=Get_timer();
					//zapal pomarañczow¹ diodê
					LED_1_ON;
				}
			}
			else if(state == RUN){
				//tyb pracy
				//NEUTRAL
				if((throttle_position >= (CENTER - BLIND_RANGE)) && (throttle_position <= (CENTER + BLIND_RANGE))){
					//zapal pomarañczow¹ diodê
					LED_1_ON;
					LED_0_OFF;
					if(throttle_direction != N){
						throttle_direction= N;
					}
					throttle_percentage=0;
				}
				//REV
				else if(throttle_position < (CENTER - BLIND_RANGE)){
					LED_1_OFF;
					if(throttle_direction != REV){
						throttle_direction= REV;
					}
					if(throttle_position > (CENTER - ADC_MAX)){
					//zapal czerwon¹ diodê
						LED_2_OFF;
						//obliczanie procentowe wychylenia dŸwigni
						throttle_percentage = -(THROTTLE_RANGE) + (throttle_position -(CENTER - ADC_MAX));
						throttle_percentage *= 100;
						throttle_percentage /= THROTTLE_RANGE;
						
					}
					else{//MAX FWD
						throttle_percentage = -100;
						LED_2_ON;
					}
					
				}
				//FWD
				else if(throttle_position > (CENTER + BLIND_RANGE)){
					//zapal zielon¹ diodê
					LED_1_OFF;
					LED_0_ON;
					LED_2_OFF;
					
					if(throttle_direction != FWD){
						throttle_direction= FWD;
					}
					//MAX_RWD
					if(throttle_position < (CENTER + ADC_MAX)){
					//zapal czerwon¹ diodê
						LED_2_OFF;
						//obliczanie procentowe wychylenia dŸwigni TBD
						throttle_percentage = (throttle_position -(CENTER + ADC_MAX)) + THROTTLE_RANGE;
						throttle_percentage *= 100;
						throttle_percentage /= THROTTLE_RANGE;
					}
					else{//MAX RWD
						throttle_percentage = 100;
						LED_2_ON;
					}
				}
				//OUT OF RANGE
				else{
					LED_2_OFF;
					LED_1_OFF;
					LED_0_OFF;
				}
				//Przeliczanie momentu
				if(Time_elapsed(time_stamp) > TORQUE_DELAY){
					if(throttle_direction != direction && direction == FWD && target_torque > 0){ //jeœli jest inny kierunek wyzeruj moment FWD
						target_torque--;		
					}
					else if(throttle_direction != direction && direction == REV && target_torque < 0){ //jeœli jest inny kierunek wyzeruj moment REV
						target_torque++;
					}
					else if(target_torque > throttle_percentage){
						target_torque--;
					}
					else if(target_torque < throttle_percentage){
						target_torque++;
					}
					
					time_stamp=Get_timer();
				}
				
				if((throttle_direction != direction) && (CAN.motor_speed < MAX_CHANGE_DIR_SPEED)){
					if(throttle_direction == FWD){
						Inv_Set_FWD();
						direction=FWD;
					}
					else if(throttle_direction == REV){
						Inv_Set_REV();
						direction=REV;
					}
					else{
						Inv_Set_N();
						direction=N;
					}
				}
				//Pozosta³e dzia³ania
				CAN.torque = target_torque*10;
				CAN.speed = MAX_SPEED;
			}
		}
    }
};

void LED_blinking(uint8_t led_0, uint8_t led_1, uint8_t led_2){
	static uint16_t led_time_stamp=0;
	static uint8_t led_status=0;
	
		if(Time_elapsed(led_time_stamp)>LED_BLINKING_PERIOD){
			
			led_time_stamp = Get_timer();
			
			if(led_status == 0){
				if(led_0)
				LED_0_ON;
				if(led_1)
				LED_1_ON;
				if(led_2)
				LED_2_ON;
				led_status = 1;
			}
			else{
				if(led_0)
				LED_0_OFF;
				if(led_1)
				LED_1_OFF;
				if(led_2)
				LED_2_OFF;
				led_status = 0;
			}
		}
};

void LEDs_OFF(void){
	LED_0_OFF;
	LED_1_OFF;
	LED_2_OFF;
	
};

void Init_HAL(void){
	
	//w³¹czenie PLL na 64MHz
	PLLCSR |= (1<PLLF) | (1<<PLLE);
	
	//ustawienie porty na wyjœciowe dla LED-ow
	DDR(LED_PORT_0) |= (1<<LED_0);
	DDR(LED_PORT_1) |= (1<<LED_1);
	DDR(LED_PORT_2) |= (1<<LED_2);
	
	LED_0_ON;
	LED_1_ON;
	LED_2_ON;
	
	/*
	//port guzika na wejœcie
	DDR(BTN_PORT) &= ~(1<<BTN);
	BTN_PULL_UP_ON;
	
	//port wejœcia cyfrowego na wejœcie
	DDR(D_IN_PORT) &= ~(1<<D_IN_PIN);
	D_IN_PULL_UP_ON;*/
	
};

void Inv_Set_FWD(){
	CAN.control_word &= ~(0x0600);
	CAN.control_word |= 0x0400;
};

void Inv_Set_N(){
	CAN.control_word &= ~(0x0600);
};
	
void Inv_Set_REV(){
	CAN.control_word &= ~(0x0600);
	CAN.control_word |= 0x0200;
};

	