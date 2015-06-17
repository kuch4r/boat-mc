//Dekalaracje funkcji
SIGNAL(SIG_ADC);
void init_adc(void);
uint16_t raw_adc(void);
uint16_t adc_get(uint8_t channel);
void adc_task(void);

#define ADMUX_MASK 0x07

#define SET_ADC_IRQ_FLAG adc_irq_flag=1;
#define CLEAR_ADC_IRQ_FLAG adc_irq_flag=0;

#define FILTER_SAMPELS 3