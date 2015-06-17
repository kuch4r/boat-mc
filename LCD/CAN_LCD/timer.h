#ifndef TIMER_H_
#define TIMER_H_

//Dekalaracje funkcji
void     init_rtc(void);
uint8_t  timer_new_tick(void);
uint16_t timer_get(void);
void     timer_init_0(void);
uint16_t timer_get_rpm(void);
uint16_t timer_time_elapsed(uint16_t time_stamp);
void     timer_0_set_pwm(uint8_t pwm);

#endif