///////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "ADC.h"
#include "makra.h"
#include "harddef.h"
#include "CAN.h"
#include "main.h"
#include "timer.h"


extern struct CAN_str CAN;

static volatile uint8_t ADC_count=0;//licznik wywo³añ funckji przerwania ADC
static volatile uint8_t ADC_IRQ_flag=0;//flaga ustawiana przez przerwanie ADC
static uint16_t buf[ADC_SAMPELS];//tablica na próbki z odczytu ADC
static uint8_t conversion_completed_flag=0;
volatile uint16_t result=0;
volatile uint16_t last_conversion_time=0;

volatile uint8_t counter=0;
volatile uint16_t commutation_period=0;
volatile uint16_t commutation_table[2];
volatile uint16_t zero_crossing_time_prev=0;
volatile uint32_t period_sum=0;
volatile uint8_t period_sum_counter=0;
extern volatile uint8_t state;


SIGNAL(SIG_ADC);

void Init_ADC(void){
	
	//Wybór Ÿród³a wyzwalania - 
	ADCSRB |= (1<<ADHSM) | (1<<AREFEN)| (0<<ADTS3) | (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
	
	// ADEN - aktywowanie ADC
	// ADATE - zewnêtrzne Ÿród³o wyzwalania
	// ADPSX - ustawienie preskalera 0 1 1 - clk/8 ->2Mhz
	// ADIE - aktywowanie przerwania
	// ADSC - start pomiaru
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	
		
	// REFS1	REFS0
	// 0		0	-nap odniesienia z VREF
	// 1		1	-nap odniesienia z wew referencji
	// MUX2..0  -wybór wejscia przetwornika			
	ADMUX |= (0<<REFS1) | (1<<REFS0);
	
	//ustawienie portu jako wejœcie
	DDR(POT_IN_PORT) &= ~(1<<POT_IN);
}

//ustawia synchronizacjê, dozwolne 0-2
void ADC_set_PSC_sync(uint8_t nr){
	
	ADCSRB = (ADCSRB & 0xF0) | ((nr + 0x07) & 0x0F);
}

//funckja zwraca uœrednion¹ wartoœæ z FILTER_SAMPELS ostaniego kana³u
uint16_t ADC_get(){
	return result;
}

void ADC_switch(uint8_t channel){
		ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
}

void ADC_force_conversion(){
	ADCSRA |= (1<<ADSC);
}

uint8_t ADC_conversion_completed(){
	if(conversion_completed_flag >= 2){
		CLEAR_CONVERSION_COMPLETED_FLAG;
		return TRUE;		
	}
	else{
		return FALSE;
	}
}

//funckja przepisuje wartoœæ z bufora poœredniego do bufora odpowiedniej wartoœci
void ADC_task(void){
	
	if(ADC_IRQ_flag){
		
		//buf[ADC_count] = (uint16_t) ADCL;// przepisz zawartoœæ rejestrów ADC do odpowiedniego miejsca w tablicy
		//buf[ADC_count] |= (uint16_t) (ADCH<<8);
		buf[ADC_count] = ADC;
			
		CLEAR_ADC_IRQ_FLAG;
		if(ADC_count>= ADC_SAMPELS-1){
			ADC_count=0;
			SET_CONVERSION_COMPLETED_FLAG;
		}
		else{
			ADC_count++;
		}
	}
	else if(conversion_completed_flag == 1){
		uint8_t i=0;
		result=0;
		
		for(i=0 ;i < ADC_SAMPELS ;i++){
			result += buf[i];
		}
		result /= ADC_SAMPELS;
		conversion_completed_flag++;
	}

};

//przewanie wywo³ywane po zakoñczeniu konwersji
SIGNAL(ADC_vect){
	//uint16_t temp=0;
	//static uint8_t phase_prev=0;
	
	SET_ADC_IRQ_FLAG;
	
	/*ATOMIC_BLOCK(ATOMIC_RESTORESTATE){		
		SET_ADC_IRQ_FLAG;
		if(ADC_count<ADC_SAMPELS-1){
			ADC_force_conversion();
		}
		else{
			last_conversion_time=Get_timer_us();
		}
		buf[ADC_count] = ADC;
			
		CLEAR_ADC_IRQ_FLAG;
		if(ADC_count>= ADC_SAMPELS-1){
			ADC_count=0;
			SET_CONVERSION_COMPLETED_FLAG;
		}
		else{
			ADC_count++;
		}
	
		if(conversion_completed_flag == 1){
			uint8_t i=0;
			result=0;
		
			for(i=0 ;i < ADC_SAMPELS ;i++){
				result += buf[i];
			}
			result /= ADC_SAMPELS;
		
		
			if(state == ON && PSC_get_current_phase() != phase_prev ){
				//CAN_send(result,PSC_get_current_phase()%2 );
				if(((result > (TRIGGER_LEVEL_LOW + PSC.torque*4)) && PSC_get_current_phase()%2==0) || ((result < (TRIGGER_LEVEL_LOW + PSC.torque*4 )) && PSC_get_current_phase()%2==1)){
						temp=last_conversion_time;
						commutation_table[counter]=commutation_period;
						if(counter<1){
							counter++;
						}
						else{
							counter=0;
						}
						commutation_table[counter] = (Time_difference_us(zero_crossing_time_prev, temp) + 3*commutation_period)/4;
						commutation_period=commutation_table[counter];
						//Set_commutation_time_prev((commutation_period*COMMUTATION_DELAY)/60, temp);
						Set_commutation_time_prev(commutation_period/2, temp);
						//Set_error_time((commutation_period*COMMUTATION_DELAY)/30);
						Set_error_time(commutation_period);
				
						zero_crossing_time_prev = temp;
				
						//obliczanie PRM-ów
						if(period_sum_counter<6){
							period_sum +=commutation_period;
							period_sum_counter++;
						}
						else{
							CAN.motor_speed = 1500000/(period_sum*POLES_NUMBER);
							period_sum=0;
							period_sum_counter=0;
						}
						phase_prev=PSC_get_current_phase();
				
				
				}
			}							
		
		}
	}	
	//zdjemij flagê przerwania
	//CLEAR_ADIF_FLAG;
	*/
}
