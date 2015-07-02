// Routines for pwm controll
//
// created : 3 Nov 2005
// Modified by: Murray Horn

#include <inttypes.h>
#include <avr/io.h>
#include "pwm001.h"
#include "harddef.h"

void	pwm_init(void)
{
    
	// fast 8bit pwm, phase correct
	TCCR1A |= (0<<COM1A1)  | (1<<WGM11);
//	TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM12) | _BV(WGM11); // the inverted pin action
    
	TCCR1B = (0<<CS12) | (1<<CS11) |(1<<CS10) | _BV(WGM13) | _BV(WGM12);
	// fast 8bit pwm prescaled freq  = xtal/( 2 * N * ICR1)

// set the port pin and direction
	DDRD = (1<<5) | DDRD;
	//PORTD = (0xff ^ (1<<5)) & PORTD;

// set max pwm value (this also affects the PWM frequency)
	ICR1 = 0x78; //- tutaj wipsaæ do ilu ma liczyæ licznik
};


void	pwm_duty_cycle(uint16_t w)
{
	OCR1AH = (0xff & (w >> 8));
	OCR1AL = (0xff & w);
};
