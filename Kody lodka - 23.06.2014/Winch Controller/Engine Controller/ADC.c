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
volatile uint8_t present_channel=0;
volatile uint8_t active_channels[]=ADC_ACTIVE_CHANNELS;

struct ADC_str ADC_RAW;



SIGNAL(SIG_ADC);

void Init_ADC(void){
	
	//Wybór Ÿród³a wyzwalania - 
	ADCSRB |= (1<<ADHSM) | (1<<AREFEN)| (0<<ADTS3) | (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
	
	// ADEN - aktywowanie ADC
	// ADATE - zewnêtrzne Ÿród³o wyzwalania
	// ADPSX - ustawienie preskalera 0 1 1 - clk/8 ->2Mhz
	// ADIE - aktywowanie przerwania
	// ADSC - start pomiaru
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (0<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
	
		
	// REFS1	REFS0
	// 0		0	-nap odniesienia z VREF
	// 1		1	-nap odniesienia z wew referencji
	// MUX2..0  -wybór wejscia przetwornika			
	ADMUX |= (0<<REFS1) | (1<<REFS0);
	
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
		ADMUX = (ADMUX & 0xF0) | (channel & ADMUX_MASK);
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
	
	if(ADC_IRQ_flag && conversion_completed_flag == 0 ){
		
		//buf[ADC_count] = (uint16_t) ADCL;// przepisz zawartoœæ rejestrów ADC do odpowiedniego miejsca w tablicy
		//buf[ADC_count] |= (uint16_t) (ADCH<<8);
		buf[ADC_count] = ADC;
			
		CLEAR_ADC_IRQ_FLAG;
		if(ADC_count >= ADC_SAMPELS-1){
			ADC_count=0;
			conversion_completed_flag++;
		}
		else{
			ADC_count++;
		}
	}
	else if(conversion_completed_flag == 1){
		uint8_t i=0;
		result=0;
		//sumowanie z odrzuceniem pierwszego wyniku
		for(i=1 ;i < ADC_SAMPELS ;i++){
			result += buf[i];
		}
		result /= (ADC_SAMPELS - 1);
		conversion_completed_flag++;
	}
	else if(conversion_completed_flag == 2){
		switch (present_channel)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			ADC_RAW.water_raw = result;
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			ADC_RAW.supply_voltage_raw = result;
			break;
		case 10:
			ADC_RAW.current_raw = result;
			break;
		default:
			break;
		}
		
		do 
		{
			if(present_channel < (ADC_CHANNELS_COUNT - 1)){
				present_channel++;
			}
			else{
				present_channel=0;
			}
		} while (active_channels[present_channel] == 0);
		CLEAR_CONVERSION_COMPLETED_FLAG;
	}

};

//przewanie wywo³ywane po zakoñczeniu konwersji
SIGNAL(ADC_vect){
	if((ADMUX & ADMUX_MASK) != present_channel){
		ADC_switch(present_channel);
	}
	else{
		SET_ADC_IRQ_FLAG;
	}
	ADC_force_conversion();
	
};
