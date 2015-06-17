//////////////////////////////////////////////////////////////////////////////////////////////////
// makra.h - makra pomocnicze 
// 
// Autor: Rados³aw Koppel          Kompilator: WinAVR 20050214
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MAKRA_H_INCLUDED
#define MAKRA_H_INCLUDED

// Makra upraszczaj¹ce dostêp do portów
// *** Port
#define PORT(x) XPORT(x)
#define XPORT(x) (PORT##x)
// *** Pin
#define PIN(x) XPIN(x)
#define XPIN(x) (PIN##x)
// *** DDR
#define DDR(x) XDDR(x)
#define XDDR(x) (DDR##x)

// NOPek
#define NOP() {asm volatile("nop"::);}

#define LED_L_ON PORT(LED_L_PORT) &= ~(_BV(LED_L))
#define LED_L_OFF PORT(LED_L_PORT) |= _BV(LED_L)


#define LED_1_ON PORT(LED_PORT) |= _BV(LED_1)
#define LED_1_OFF PORT(LED_PORT) &= ~(_BV(LED_1))


#define LED_H_OFF PORT(LED_H_PORT) &= ~(_BV(LED_H))
#define LED_H_ON PORT(LED_H_PORT) |= _BV(LED_H)



#define ADC_V_AKU ADMUX = (ADMUX & 0xE0) | 0x06;


#endif //MAKRA_H_INCLUDED
