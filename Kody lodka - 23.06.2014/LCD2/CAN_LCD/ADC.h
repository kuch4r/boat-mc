//Dekalaracje funkcji
SIGNAL(SIG_ADC);
void Init_ADC(void);
uint16_t Raw_ADC(void);
uint16_t ADC_get(uint8_t channel);
void ADC_task(void);

#define ADMUX_MASK 0x07

#define SET_ADC_IRQ_FLAG ADC_IRQ_flag=1;
#define CLEAR_ADC_IRQ_FLAG ADC_IRQ_flag=0;

#define FILTER_SAMPELS 3