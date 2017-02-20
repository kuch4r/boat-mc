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
#include "main.h"
#include "timer.h"
#include "EEPROM.h"


extern struct CAN_str CAN;
extern struct eeprom_struct non_volatile_data;


volatile uint8_t present_channel=0;
volatile uint8_t active_channels[]=ADC_ACTIVE_CHANNELS;

volatile struct adc_state adc_state;
struct adc_data  adc_results;

// translate internal index to ADC channel num
volatile uint8_t channel_idx_to_num[ADC_MAX_CHANNEL];

SIGNAL(SIG_ADC);

void ADC_init(void){
	int8_t c;
	
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
	
	// init adc state struct
	adc_state.active_channel  = 0;
	adc_state.buffor_position = 0;
	
	//translate index to ADC channel num
	channel_idx_to_num[ADC_CHANNEL_BOARD_POSITION]   = ADC_CHANNEL_BOARD_POSITION_NUM;
	channel_idx_to_num[ADC_CHANNEL_VOLTAGE] = ADC_CHANNEL_VOLTAGE_NUM;
	channel_idx_to_num[ADC_CHANNEL_CURRENT] = ADC_CHANNEL_CURRENT_NUM;
	
	// init sum
	for( c = 0 ; c < ADC_MAX_CHANNEL ; c++ ){
		adc_state.sum[c] = 0;
	}

	// trun on first channel	
	ADC_switch(channel_idx_to_num[adc_state.active_channel]);
	
	adc_ready_to_send = 0;
}

void ADC_switch(uint8_t channel){
		ADMUX = (ADMUX & 0xF0) | (channel & ADMUX_MASK);
}

void ADC_start_conversion(){
	ADCSRA |= (1<<ADSC);
}

// ADC task function, called every loop in main_loop
void ADC_task(void){
	/* if we have new results */
	if( HAS_FLAG(adc_state.flags, ADC_FLAG_CONV_COMPLETED)) {
		adc_results.raw_current = adc_state.sum[ADC_CHANNEL_CURRENT] / ADC_SAMPELS;
		adc_results.raw_voltage = adc_state.sum[ADC_CHANNEL_VOLTAGE] / ADC_SAMPELS;
		adc_results.raw_board_position   = adc_state.sum[ADC_CHANNEL_BOARD_POSITION]   / ADC_SAMPELS;		
		
		/* voltage result */
		if((adc_results.raw_voltage + non_volatile_data.adc_voltage_offset) < 1024 && (non_volatile_data.adc_voltage_offset + adc_results.raw_voltage) > 0){
			adc_results.voltage = ((((uint32_t)adc_results.raw_voltage + non_volatile_data.adc_voltage_offset))*non_volatile_data.adc_voltage_scale) / 1024;
		}
		else {
			adc_results.voltage = 0;
		}
		
		/* current result */
		if(((adc_results.raw_current + non_volatile_data.adc_current_offset) < 1024) && ((non_volatile_data.adc_current_offset + adc_results.raw_current) > 0)){
			adc_results.current=((((uint32_t)adc_results.raw_current + non_volatile_data.adc_current_offset))*non_volatile_data.adc_current_scale) / 1024; //przeliczanie ADU na pr¹d (ADU*I_SCALE)/1024 i wyliczanie œredniej z poprzednim pomiarem
		} else {
			adc_results.current = 0;
		}
		
		/* board position result */
		if( adc_results.raw_board_position <= non_volatile_data.adc_board_position_min) {
			adc_results.board_position = 0;
		} else if( adc_results.raw_board_position >= non_volatile_data.adc_board_position_max ) {
			adc_results.board_position = 100;
		} else {
			adc_results.board_position = (((uint32_t)adc_results.raw_board_position - non_volatile_data.adc_board_position_min) * 100) / (non_volatile_data.adc_board_position_max - non_volatile_data.adc_board_position_min);
		}
		
		adc_ready_to_send++;
		/* clearing flag for conversion resulsts */
		CLEAR_FLAG(adc_state.flags, ADC_FLAG_CONV_COMPLETED);
	}
};

// ADC conversion completed signal
SIGNAL(ADC_vect){
	/* save result */
	
	/* removing old result from the sum */
	if( HAS_FLAG(adc_state.flags, ADC_FLAG_HAS_FULL_BUFF) ) {
		adc_state.sum[adc_state.active_channel] -= adc_state.buffor[adc_state.active_channel][adc_state.buffor_position];
	}
	/* putting new result to the buffor */
	adc_state.buffor[adc_state.active_channel][adc_state.buffor_position] = ADC;
	/* adding new result to sum */
	adc_state.sum[adc_state.active_channel] += adc_state.buffor[adc_state.active_channel][adc_state.buffor_position];
	
	/* inc channel position counters */
	if( ++adc_state.active_channel == ADC_MAX_CHANNEL) {
		/* we checked all channels, starting from the begging */
		adc_state.active_channel = 0;
		/* reseting buffer position */
		if( ++adc_state.buffor_position == ADC_SAMPELS ){
			/* we filled whole buffor, starting from the begging */
			adc_state.buffor_position = 0;
			// setting flag that buffor is full (you can read results)
			SET_FLAG(adc_state.flags, ADC_FLAG_HAS_FULL_BUFF);
		}
		/* setting flag that there is new result ready */
		if( HAS_FLAG(adc_state.flags, ADC_FLAG_HAS_FULL_BUFF) ) {
			SET_FLAG(adc_state.flags, ADC_FLAG_CONV_COMPLETED);
		}
	}
	/* switching active channel */
	ADC_switch(channel_idx_to_num[adc_state.active_channel]);
	/* starting conversion */
	ADC_start_conversion();	
};
