
// Wersja dzialajaca, od wiosny 2010 w lodce
//brak oblugi wyswietalcza graficznego
//nie przetestowane dzialanie pochdnej w zabapieczeniu
//zabezpieczenie nadpradowe dziajace



// Includes
#include <inttypes.h>
#include <avr/io.h>			// included to enable the writing of strings from rom
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "harddef.h"
#include "makra.h"
#include <util/delay.h>
#include "pwm001.h"
#include "delay.h"
 

   

//#define F_CPU 1200000      /* CPU speed */




//-----------------------------------------------------
// Main - a simple test program


volatile uint8_t delay=50;
volatile uint8_t licznik = 0;
volatile uint8_t pomiary[3];
volatile uint8_t flaga = 0;
volatile uint8_t timer = 0;

SIGNAL(TIM0_OVF_vect);



int main( void )
{

	uint8_t temp = 0;
	uint8_t target_delay = 0;
	uint8_t direction = N;
	uint8_t prev_direction = N;
	
	//ustawienie kierunku portów na wyjœcia
   	DDR(LED_PORT) = (1<<LED_1) | (1<<LED_2) |(1<<PWM);
	DDR(LED_PORT) &= ~(1<<4);
	PWM_HIGH;
	
	//inicjacje timera
	TIMSK0 = (0<<OCIE0A) | (0<<OCIE0B) | (1<<TOIE0);
	TCCR0A = (1<<WGM01) | (1<<WGM00) | (1<<COM0A1) | (1<<COM0A0);
	TCCR0B = (0<<CS02) | (0<<CS01) | (1<<CS00);
	OCR0A = 0x05;
	OCR0B = 0x5E;
	//OCR0A = 0xFF;
	
	//ACSR |= (1<<ACD);
	
	//Inicajlizacja ADC
	
	ADCSRA = (1<<ADEN) | (1<<ADSC) |  (1<<ADATE)| (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
	ADMUX = (0<<REFS0) | (1<<MUX1) | (0<<MUX0) | (1<<ADLAR);
	ADCSRB = (1<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
	
	sei();
	
	//oczekiwanie na inicjalizacje reglera
	delay = CENTER;
	LED_1_ON;
	LED_2_ON;
	while(timer<20){;}
	LED_2_OFF;
	//oczekuj na pozyje zerow¹
	do{
		temp = (uint8_t)(((uint16_t)pomiary[0]+(uint16_t)pomiary[1]+(uint16_t)pomiary[2])/3);
	} while(!(temp > 128-BLIND_RANGE && temp < 128 + BLIND_RANGE && direction == N));
	
	//pêtla g³ówna
	while (1)
	{
		//flaga wywo³ania obliczeñ (po uaktualnieniu pomiaru z ADC
		if(timer>100){
			timer = 0;
			//wyliczenie œredniej z trzech pomiarów
			temp = (uint8_t)(((uint16_t)pomiary[0]+(uint16_t)pomiary[1]+(uint16_t)pomiary[2])/3);
			
			//wy³¹czenie diód
			LED_2_OFF;
			//jesli ADC w polu neutralnym to wysterowanie 0					histereza
			if((temp > 128-BLIND_RANGE && temp < 128 + BLIND_RANGE && direction == N) || (temp > 128-(BLIND_RANGE-1) && temp < 128 + (BLIND_RANGE-1) && direction != N)){
				target_delay = CENTER;
				LED_2_ON;
				//direction=N;
				}
			//Jeœli ADC w polu do przodu wystaw moment dodatni
			else if(temp > 128){
				LED_1_OFF;
				//zabezpiecznie przed wartoœci¹ ujemn¹
				//zapal diodê - pe³en GAZ
				if(temp>208){
					temp=208;
					LED_2_ON;
					}
				target_delay = (((uint16_t)temp-(128+BLIND_RANGE))*32)/9;
				direction=REV;
				
			}//Jeœli ADC w polu do ty³u wystaw moment ujmeny
			else{
				
				//zabezpiecznie przed wartoœci¹ ujemn¹
				//zapal diodê - pe³en GAZ
				if(temp<48){
					temp=48;
					LED_2_ON;
				}
					
				target_delay = (((128-BLIND_RANGE)-(uint16_t)temp)*32)/9;
				direction=FWD;
			}
			
			
			//wyg³adzanie momentu
			if(target_delay != delay){
				if(direction != prev_direction && delay != CENTER){
						pwm_duty_cycle(delay--);
				}
				else{
					prev_direction=direction;
					if(direction ==FWD)
						LED_1_ON;
					if(target_delay != delay){
						if(target_delay > delay)
							pwm_duty_cycle(delay++);
						else
							pwm_duty_cycle(delay--);
					}
				}
			}
		}
		
	}
}

SIGNAL(TIM0_OVF_vect){
	pomiary[licznik]=0xFF-ADCH;
	if(++licznik >= 3)
		licznik=0;
	timer++;
}



	