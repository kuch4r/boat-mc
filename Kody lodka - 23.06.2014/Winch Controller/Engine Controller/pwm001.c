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
    
	
};


void pwm_duty_cycle(uint8_t w)
{
	OCR0A = w;
};

void pwm_set_top(uint8_t w){
	
	OCR0A = w;

};
