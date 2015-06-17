//////////////////////////////////////////////////////////////////////////////////////////////////
// harddef.h - definicja sprzêtu dla programu PCF8591, p³ytka AVT3500
// 
// Autor: Rados³aw Koppel          Kompilator: WinAVR 20050214
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef HARDDEF_H_INCLUDED
#define HARDDEF_H_INCLUDED


#define LED_PORT B
#define LED_1 2

#define LED_PORT B
#define LED_2 3

#define PWM_PORT B
#define PWM 0

//mkasymalna moc [0-40] [12,5us]
#define MAX 40

//moc minmalna, ponizej tego jest zero [0-40] [12,5us]
#define MIN 4

//okreœla poziom ADC przy którym ustawiany jest maksymalny gaz [0-64 ADC]
#define ADC_MAX 45

//Zakres przy ktróym poziom gazu =0 [0-64 ADC]
#define BLIND_RANGE 10

//œrodek [0-80] [12,5us]
#define CENTER 7

#define FWD 1
#define N 0
#define REV 2

#define DOUBLE_CLICK_DURATION 23





#endif