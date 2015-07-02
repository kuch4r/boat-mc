///////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////
#include <avr\io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "makra.h"
#include "harddef.h"

SIGNAL(SIG_OVERFLOW2);
SIGNAL(SIG_INPUT_CAPTURE1);
SIGNAL(SIG_OVERFLOW1);
static volatile uint16_t timer=0; //czas w [7,8125ms]
static volatile uint16_t timer_prv = 0; // wartoœæ timera w poprzednim cyklu;
static volatile uint8_t new_tick=0; // jeœli 1 to znaczy ¿e w tej interacji nowy tick - up³yne³o 7,8125 ms
static volatile uint16_t rpm_timer=0;

void Init_RTC(void){ //real time clock
	//praca asynhroniczna
    ASSR |= (1<<AS2);
	//wartoœci pocz¹tkowe rejestrów przerwan
    //TIFR |= (1<<TOV2)|(1<<OCF2);
	// WGM - praca normalna PWM
	//CS2..0 - ustwienie preskalera
	//CS22	CS21	CS20 - clk = 32,768kHZ = 128*256
	//0		0		0	- timer stoped
	//0		0		1	- clk/1
	//0		1		0	- clk/8
	//0		1		1	- clk/32
	//1		0		0	- clk/64
	//1		0		1	- clk/128
	//1		1		0	- clk/256
	//1		1		1	- clk/1024
	    TCCR2A |= (0<<WGM20)|(0<<WGM21)|(0<<CS22)|(0<<CS21)|(1<<CS20);
	//w³aczenie zg³aszania przerwania przy przepe³nieniu
    TIMSK2 |= (1<<TOIE2);
	//kierunki portów dla zewn kwarcu
	DDRG |= (1<<3) | (1<<4);

};

void Init_Timer0(void){
	// WGM normal 16 bit mode
	TCCR0A |= (0<<WGM11) | (1<<WGM10) | (1<<COM0A1)| (0<<COM0A0) | (1<<CS02) | (0<<CS01) | (1<<CS00);
    
	//CS preskaler 64 f=93750Hz
	//ICNC1 w³¹czenie noise canceler
	//WGM normal mode
	//ICES1 - 0 zbocze odapaj¹ce
	//TCCR0B |= (0<<CS12) | (1<<CS11) | (1<<CS10) | (0<<WGM13) | (0<<WGM12) | (1<<ICNC1) | (0<<ICES1);
	
	//TICIE1 - w³¹czenie przerwania Input Capture
	//TOIE1 - w³¹czenie przerwania od przepe³nienia licznika jeden - potrzebne do wykrywania predkoœci 0
	//TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);

};
  
void Timer0_Set_PWM(uint8_t pwm){
	OCR0A = pwm;
	
}
//funckje zwraca TRUE jeœli od jej ostatniego wywo³ania up³yne³o 7,8125ms
inline uint8_t Timer_new_tick(void){

	timer_prv=timer;
	
	if(timer!=timer_prv){
			return 1;
		}
		else{
			return 0;
		}
		
		
}
//funkcja zwraca akutaln¹ wartoœc timera RTC
inline uint16_t Get_timer(){
	return timer;
}

//zwraca czas w [1,3ms] jaki czas up³yn¹³ od wartoœci podanej jako argument (argument czas timera)
//Mierzy czas do 8,5 minuty!! potem siê przewija licznik!!
uint8_t Time_elapsed(uint16_t time_stamp){
	uint16_t current_timer=Get_timer();
	
	if( current_timer >= time_stamp){
		return (current_timer-time_stamp);
	}
	else{
		return (current_timer+(0xFFFF-time_stamp));
	}
}

uint16_t Get_rpm(void){
	uint16_t rpm=0;
	
		if(rpm_timer > 100){
			rpm_timer /= 6;	
			rpm = (FCPU/2) / rpm_timer;
			rpm *= 20;
		}
		
	return rpm;
}



SIGNAL(SIG_OVERFLOW2) {
	//zwiêksz TIMER
	timer++;
}

SIGNAL(SIG_INPUT_CAPTURE1){
	
	TCNT1 = 0x0000;
	rpm_timer=ICR1;

};

SIGNAL(SIG_OVERFLOW1){
	rpm_timer = 1;
};
