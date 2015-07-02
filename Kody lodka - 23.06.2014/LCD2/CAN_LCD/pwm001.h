

#define PWM_OFF TCCR1A &= ~(1<<COM1A1);\
		PORTD &= ~(1<<PD5);
				
#define PWM_ON TCCR1A |= (1<<COM1A1);
		


void	pwm_init(void);

void	pwm_duty_cycle(uint16_t w);
