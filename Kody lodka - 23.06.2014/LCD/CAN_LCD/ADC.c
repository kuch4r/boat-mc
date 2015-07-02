/////////////////////  //////////////////////////////////////////////////////////////////////////
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "makra.h"
#include "harddef.h"

static uint8_t          adc_mux      = 0; //zmienna przechowuj�ca aktulane wej�cie przetwornika ADC
static uint16_t         adc_sample   = 0; //licznik pomocniczy umozliwiaj�cy zapis sampli do odpwiedniego miejsca w tablicy
static volatile uint8_t adc_count    = 0; //licznik wywo�a� funckji przerwania ADC
static volatile uint8_t adc_irq_flag = 0; //flaga ustawiana przez przerwanie ADC
static uint16_t         adc_buf[8][FILTER_SAMPELS]; //tablica na pr�bki z odczytu ADC

SIGNAL(SIG_ADC);

void init_adc(void){
	// ADEN - aktywowanie ADC
	// ADATE - zewn�trzne �r�d�o wyzwalania
	// ADPSX - ustawienie preskalera 1 1 1 - clk/128
	// ADIE - aktywowanie przerwania
	// ADSC - start pomiaru
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | (1<<ADATE) | _BV(ADSC) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);
	
	//Wyb�r �r�d�a wyzwalania - free ruunig
	ADCSRB |= (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
	
	//kierunek pin�w
	DDR(ADC_6_PORT) &= ~(1<<ADC_6) ;
	
	// REFS1	REFS0
	// 0		0	-nap odniesienia z VREF
	// 1		1	-nap odniesienia z wew referencji
	// MUX2..0  -wyb�r wejscia przetwornika			
	ADMUX |= (0<<REFS1) | (1<<REFS0);
	ADC_V_AKU; //pocz�tkowe ustawienie wej�cia
	
}

/*
 * funckja zwraca u�rednion� warto�� z FILTER_SAMPELS ostaniego kana�u
 */
uint16_t adc_get(uint8_t channel){
	uint16_t sum = 0;
	uint8_t  i   = 0;
	
	for( i = 0; i < FILTER_SAMPELS; i++ ){
		sum += adc_buf[channel][i];
	}
	return sum / FILTER_SAMPELS;
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

//funckja przepisuje warto�� z bufora po�redniego do bufora odpowiedniej warto�ci
void adc_task(void){
	if(adc_irq_flag){
		//uart_dec(ADC_mux);	
		adc_buf[adc_mux][adc_sample] = (uint16_t) ADCL;// przepisz zawarto�� rejestr�w ADC do odpowiedniego miejsca w tablicy
		adc_buf[adc_mux][adc_sample] |= (uint16_t) (ADCH<<8);
		
		/*if((ADMUX & ADMUX_MASK) == 0){ // je�li �sme wej�cie przetwornika 
			ADC_V_AKU; //prze��cz na kana� zerowy
			ADC_mux=0;	//zeruj licznik pomocniczy
			if(ADC_sample >= (FILTER_SAMPELS-1)){ //je�li przelecia�y wszystkie wejscia prze��cz na nast�ne miejsce w tablicy pr�bek
				ADC_sample = 0;
			}
			else{
				ADC_sample++;
			}
		}
		
		else{				//zmie� na nast�pny kana�
			ADMUX &= 0xE0; 
			ADMUX |= (++ADC_mux);
		}*/
		
		adc_irq_flag = 0;
	}

};

//przewanie wywo�ywane po zako�czeniu konwersji
SIGNAL(SIG_ADC){
	if( adc_count >= 1){
		adc_irq_flag = 1;
		adc_count    = 0;
	} else {
		adc_count++;
	}
}
