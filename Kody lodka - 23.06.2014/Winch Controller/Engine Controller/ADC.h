//Dekalaracje funkcji

#ifndef ADC_H_
#define ADC_H_
SIGNAL(SIG_ADC);

void Init_ADC(void);
uint16_t Raw_ADC(void);
uint16_t ADC_get();
void ADC_task(void);
void ADC_set_PSC_sync(uint8_t nr);
void ADC_switch(uint8_t channel);
uint8_t ADC_conversion_completed();
void ADC_force_conversion();
#define ADMUX_MASK 0x0F

#define SET_ADC_IRQ_FLAG ADC_IRQ_flag=1;
#define CLEAR_ADC_IRQ_FLAG ADC_IRQ_flag=0;

#define CLEAR_ADIF_FLAG ADCSRA &= ~(1<<ADIF);

#define SET_CONVERSION_COMPLETED_FLAG conversion_completed_flag=1;
#define CLEAR_CONVERSION_COMPLETED_FLAG conversion_completed_flag=0;

#define ADC_ENABLE_INTERRUPT ADCSRA |= (1<<ADIE);
#define ADC_DISABLE_INTERRUPT ADCSRA &= ~(1<<ADIE);

#define ADC_SAMPELS 15

struct ADC_str {
	int16_t current_raw;
	int16_t supply_voltage_raw;
	int16_t water_raw;
};


#endif