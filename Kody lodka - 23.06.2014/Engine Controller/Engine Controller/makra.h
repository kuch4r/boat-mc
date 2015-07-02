
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

#define LED_0_ON PORT(LED_PORT_0) |= _BV(LED_0)
#define LED_0_OFF PORT(LED_PORT_0) &= ~(_BV(LED_0))

#define LED_1_ON PORT(LED_PORT_1) |= _BV(LED_1)
#define LED_1_OFF PORT(LED_PORT_1) &= ~(_BV(LED_1))

#define LED_2_ON PORT(LED_PORT_2) |= _BV(LED_2)
#define LED_2_OFF PORT(LED_PORT_2) &= ~(_BV(LED_2))


#define TRUE 1
#define FALSE 0

#endif //MAKRA_H_INCLUDED
