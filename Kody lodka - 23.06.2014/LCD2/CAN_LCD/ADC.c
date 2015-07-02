///////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "makra.h"
#include "harddef.h"

static uint8_t ADC_mux=0;//zmienna przechowuj¹ca aktulane wejœcie przetwornika ADC
static uint16_t ADC_sample=0;//licznik pomocniczy umozliwiaj¹cy zapis sampli do odpwiedniego miejsca w tablicy
static volatile uint8_t ADC_count=0;//licznik wywo³añ funckji przerwania ADC
static volatile uint8_t ADC_IRQ_flag=0;//flaga ustawiana przez przerwanie ADC
static uint16_t buf[8][FILTER_SAMPELS];//tablica na próbki z odczytu ADC


SIGNAL(SIG_ADC);

void Init_ADC(void){
	// ADEN - aktywowanie ADC
	// ADATE - zewnêtrzne Ÿród³o wyzwalania
	// ADPSX - ustawienie preskalera 1 1 1 - clk/128
	// ADIE - aktywowanie przerwania
	// ADSC - start pomiaru
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | (1<<ADATE) | _BV(ADSC) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	
	
	//Wybór Ÿród³a wyzwalania - free ruunig
	ADCSRB |= (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
	
	//kierunek pinów
	DDR(ADC_6_PORT) &= ~(1<<ADC_6) ;
	
	// REFS1	REFS0
	// 0		0	-nap odniesienia z VREF
	// 1		1	-nap odniesienia z wew referencji
	// MUX2..0  -wybór wejscia przetwornika			
	ADMUX |= (0<<REFS1) | (1<<REFS0);
	ADC_V_AKU; //pocz¹tkowe ustawienie wejœcia
	
}

//funckja zwraca uœrednion¹ wartoœæ z FILTER_SAMPELS ostaniego kana³u
uint16_t ADC_get(uint8_t channel){
	uint16_t mean=0;
	uint8_t i=0;
	
	for(i=0 ;i < FILTER_SAMPELS ;i++){
		mean += buf[channel][i];
	}
	mean /= FILTER_SAMPELS;
	
	return mean;
		
}

/*void Switch_ADC(uint8_t channel){

	switch(channel) {
		case TEMP_2 :
			ADC_TEMP_2
			break;
		case SEN :
			ADC_SEN
			break;
		case V_AKU :
			ADC_V_AKU
			break;
		case DAC :
			ADC_DAC
			break;
		case V_ENG_AKU :
			ADC_V_ENG_AKU
			break;
		case I_ENG_AKU :
			ADC_I_ENG_AKU
			break;
		case I_AKU :
			ADC_I_AKU
			break;
		case TEMP_1 :
			ADC_TEMP_1
			break;
	}

}*/

//funckja przepisuje wartoœæ z bufora poœredniego do bufora odpowiedniej wartoœci
void ADC_task(void){
	if(ADC_IRQ_flag){
		//uart_dec(ADC_mux);	
		buf[ADC_mux][ADC_sample] = (uint16_t) ADCL;// przepisz zawartoœæ rejestrów ADC do odpowiedniego miejsca w tablicy
		buf[ADC_mux][ADC_sample] |= (uint16_t) (ADCH<<8);
		
		/*if((ADMUX & ADMUX_MASK) == 0){ // jeœli ósme wejœcie przetwornika 
			ADC_V_AKU; //prze³¹cz na kana³ zerowy
			ADC_mux=0;	//zeruj licznik pomocniczy
			if(ADC_sample >= (FILTER_SAMPELS-1)){ //jeœli przelecia³y wszystkie wejscia prze³¹cz na nastêne miejsce w tablicy próbek
				ADC_sample = 0;
			}
			else{
				ADC_sample++;
			}
		}
		
		else{				//zmieñ na nastêpny kana³
			ADMUX &= 0xE0; 
			ADMUX |= (++ADC_mux);
		}*/
		
		CLEAR_ADC_IRQ_FLAG;
	}

};

//przewanie wywo³ywane po zakoñczeniu konwersji
SIGNAL(SIG_ADC){
		
		if( ADC_count >= 1){
			SET_ADC_IRQ_FLAG;
			ADC_count=0;
		}
		else{
			ADC_count++;
		}
}
