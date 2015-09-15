/*
 * CFile1.c
 *
 * Created: 2014-09-06 22:26:42
 *  Author: LukaszKucharski
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "makra.h"
#include "harddef.h"
#include "button.h"
#include "display.h"

volatile uint8_t button_state;
volatile uint8_t screen_line=0;

void Buttons_Init(void){
	//Inicjalizacja portu jako wej�cie
	DDR(BTN_1_PORT) &= ~(1<<BTN_1);
	DDR(BTN_2_PORT) &= ~(1<<BTN_2);
	DDR(BTN_3_PORT) &= ~(1<<BTN_3);
	DDR(BTN_4_PORT) &= ~(1<<BTN_4);
	DDR(BTN_5_PORT) &= ~(1<<BTN_5);
	
	//w��czenie pull-up-�w
	PORT(BTN_1_PORT) |= (1<<BTN_1);
	PORT(BTN_2_PORT) |= (1<<BTN_2);
	PORT(BTN_3_PORT) |= (1<<BTN_3);
	PORT(BTN_4_PORT) |= (1<<BTN_4);
	PORT(BTN_5_PORT) |= (1<<BTN_5);
	
	//ustawienie przerwania od INT0 na zbocze opadaj�ce
	EICRA |= (0<<ISC00) | (1<<ISC01) | (0<<ISC10) | (1<<ISC11) | (0<<ISC20) | (1<<ISC21) | (0<<ISC30) | (1<<ISC31);
	
	//W��cznie przerwania INT0
	EIMSK |= (1<<INT0) | (1<<INT1) | (1<<INT2) | (1<<INT3);
	
	//W��czenie przerwania od ICP1 (BTN_5)
	TIMSK1 |= (1<<ICIE1);
	
	button_state=0;
};

uint8_t Buttons_get_screen_line(){
	return screen_line;
}

//Przerwanie od BTN_1
SIGNAL(SIG_INTERRUPT0) {
	if(screen_line < (SCREEN_LINES_COUNT * 2) - 1){
		screen_line++;
	}
	else{
		screen_line = 0;
	}
};

//Przerwanie od BTN_2
SIGNAL(SIG_INTERRUPT1) {
	//LED_1_ON;
	//button_state++;
};

//Przerwanie od BTN_3
SIGNAL(SIG_INTERRUPT2) {
	//LED_1_ON;
	//button_state++;
};

//Przerwanie od BTN_4
SIGNAL(SIG_INTERRUPT3) {
	//LED_1_ON;
	//button_state++;
};

//Przerwanie od BTN_5
SIGNAL(SIG_INPUT_CAPTURE1) {
	if(screen_line > 0){
		screen_line--;
	}
	else{
		screen_line = (SCREEN_LINES_COUNT * 2) - 1;
	}

};