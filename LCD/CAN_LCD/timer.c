///////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
///////////////////////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "makra.h"
#include "harddef.h"

SIGNAL(SIG_OVERFLOW2);
//SIGNAL(SIG_INPUT_CAPTURE1);
SIGNAL(SIG_OVERFLOW1);
static volatile uint16_t timer     = 0; // czas w [7,8125ms]
static volatile uint16_t timer_prv = 0; // warto�� timera w poprzednim cyklu;
static volatile uint8_t  new_tick  = 0; // je�li 1 to znaczy �e w tej interacji nowy tick - up�yne�o 7,8125 ms
static volatile uint16_t rpm_timer = 0;

void init_rtc(void){ //real time clock
	//praca asynhroniczna
    ASSR |= _BV(AS2);
	
	//w��czenei przerwania przy przepe�nieniu
	TIMSK2 |= (1<<OCIE2A);
	
	//ustawienie maksymalnej warto�� licznika przy kt�rej si� przepe�nia
	OCR2A = 33;
	
	//warto�ci pocz�tkowe rejestr�w przerwan
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
	TCCR2A |= (1<<WGM21)|(0<<WGM20)|(0<<CS22)|(0<<CS21)|(1<<CS20);
	
	//w�aczenie zg�aszania przerwania przy przepe�nieniu
    TIMSK2 |= (1<<TOIE2);
	
	//kierunki port�w dla zewn kwarcu
	DDRG |= (1<<3) | (1<<4);

};

void timer_init_0(void){
	// WGM normal 16 bit mode
	TCCR0A |= (0<<WGM11) | (1<<WGM10) | (1<<COM0A1)| (0<<COM0A0) | (1<<CS02) | (0<<CS01) | (1<<CS00);
    
	//CS preskaler 64 f=93750Hz
	//ICNC1 w��czenie noise canceler
	//WGM normal mode
	//ICES1 - 0 zbocze odapaj�ce
	//TCCR0B |= (0<<CS12) | (1<<CS11) | (1<<CS10) | (0<<WGM13) | (0<<WGM12) | (1<<ICNC1) | (0<<ICES1);
	
	//TICIE1 - w��czenie przerwania Input Capture
	//TOIE1 - w��czenie przerwania od przepe�nienia licznika jeden - potrzebne do wykrywania predko�ci 0
	//TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);
};
  

//funckje zwraca TRUE je�li od jej ostatniego wywo�ania up�yne�o 7,8125ms
inline uint8_t timer_new_tick(void){
	static uint16_t time_stamp=0;

	if(timer_time_elapsed(time_stamp) < MAIN_LOOP_PERIOD){
		return 0;
	}
	else{
		time_stamp = timer_get();
		return 1;
	}
}

//funkcja zwraca akutaln� warto�c timera RTC
inline uint16_t timer_get(){
	return timer;
}

//zwraca czas w [1,3ms] jaki czas up�yn�� od warto�ci podanej jako argument (argument czas timera)
//Mierzy czas do 8,5 minuty!! potem si� przewija licznik!!
uint16_t timer_time_elapsed(uint16_t time_stamp){
	uint16_t current_timer = timer_get();
	if( current_timer >= time_stamp){
		return (current_timer-time_stamp);
	} else {
		return (current_timer+(0xFFFF-time_stamp));
	}
}





/*SIGNAL(SIG_OVERFLOW2) {
	timer++;
}*/

//przerwanie przepe�niania Timer2
SIGNAL(SIG_OUTPUT_COMPARE2) {
	//zwi�ksz TIMER
	timer++;
}
