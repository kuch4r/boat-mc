
//Enables console used for development purposes only
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "UART.h"
#include "makra.h"
#include "harddef.h"
#include "main.h"
#include "timer.h"


void UART_init(void){
	
	LINCR = (1 << LSWRES); //reset
	
	LINBRRH = (((F_CPU/BAUD)/16)-1)>>8;
	LINBRRL = (((F_CPU/BAUD)/16)-1);
	
	LINBTR = (1 << LDISR) | (16 << LBT0); 
	
	LINCR |= (1<<LENA) | (1<<LCMD0)| (1<<LCMD1)| (1<<LCMD2); //UART Enable - RX and TX enable
	
	//DDR(D) |= (1<<3);
	
	for(uint16_t i=0; i<10; ){
		if((LINSIR & (1<<LBUSY)) == 0){
			LINDAT = 0xAA; //bufor danych nadawczych
			i++;
		}
		
	}

}