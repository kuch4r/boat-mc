//Dekalaracje funkcji

#ifndef ADC_H_
#define ADC_H_
SIGNAL(SIG_ADC);

void ADC_init(void);
void ADC_task(void);
void ADC_switch(uint8_t channel);
void ADC_start_conversion();

#define ADMUX_MASK 0x0F

#define CLEAR_ADIF_FLAG ADCSRA &= ~(1<<ADIF);

#define ADC_ENABLE_INTERRUPT ADCSRA |= (1<<ADIE);
#define ADC_DISABLE_INTERRUPT ADCSRA &= ~(1<<ADIE);

#define ADC_SAMPELS 15

#define ADC_MAX_CHANNEL     3

#define ADC_CHANNEL_CURRENT 0
#define ADC_CHANNEL_VOLTAGE 1
#define ADC_CHANNEL_BOARD_POSITION   2

#define ADC_CHANNEL_CURRENT_NUM 10
#define ADC_CHANNEL_VOLTAGE_NUM 9
#define ADC_CHANNEL_BOARD_POSITION_NUM   4

#define ADC_FLAG_CONV_COMPLETED 0
#define ADC_FLAG_HAS_FULL_BUFF  1

struct adc_state {
	uint8_t flags;
	uint8_t active_channel;
	uint8_t buffor_position;
	volatile int16_t buffor[ADC_MAX_CHANNEL][ADC_SAMPELS];
	volatile int16_t sum[ADC_MAX_CHANNEL];
};
struct adc_data {
	int16_t raw_current;
	int16_t raw_voltage;
	int16_t raw_board_position;
	int16_t current;
	int16_t voltage;
	int16_t board_position;
};

struct adc_data  adc_results;

#endif