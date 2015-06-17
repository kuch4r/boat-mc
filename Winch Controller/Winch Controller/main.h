/*
 * BLDC.h
 *
 * Created: 2013-02-03 23:50:15
 *  Author: LukaszKucharski
 */ 


#ifndef BLDC_H_
#define BLDC_H_

volatile uint8_t adc_ready_to_send;

void Init_HAL();
void LED_blinking(uint8_t led_0, uint8_t led_1);
void LEDs_OFF(void);
void main_loop(void);

uint8_t Get_DIP_sw_word(void);
#define INIT 0
#define INV_INIT 1
#define WAITING_FOR_ZERO 2
#define RUN 3

#define SET_FLAG(target, flag)    target = target | (1 << flag)
#define CLEAR_FLAG( target, flag) target = target & (~(1 << flag))
#define HAS_FLAG( target, flag)   (target & (0x01 << flag))



#endif /* BLDC_H_ */