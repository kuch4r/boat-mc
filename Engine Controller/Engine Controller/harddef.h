
#ifndef HARDDEF_H_INCLUDED
#define HARDDEF_H_INCLUDED

//czas co który wykonywana jest pêtla g³owna [4us]
#define MAIN_LOOP_PERIOD 25 //0,1ms

#define LED_PORT_0 B
#define LED_0 3

#define LED_PORT_1 B
#define LED_1 4

#define LED_PORT_2 B
#define LED_2 2

#define BTN_PORT C
#define BTN 5

#define D_IN_PORT D
#define D_IN_PIN 1

#define POT_IN_PORT B
#define POT_IN 5
#define POT_IN_ADC 6

//mkasymalna moc [0-40]
#define MAX_TORQUE 40

//moc minmalna, ponizej tego jest zero [0-40] 
#define MIN_TORQUE 4

#define MAX_SPEED 2500

//okreœla poziom ADC przy którym ustawiany jest maksymalny gaz [0-511 ADC]
#define ADC_MAX 300

//Zakres przy ktróym poziom gazu =0 [0-511 ADC] - liczony od CENTER w obie storny (w sumie gaz zero przez BLIND_RANGE*2)
#define BLIND_RANGE 80

#define THROTTLE_RANGE ADC_MAX - BLIND_RANGE

#define ADC_RESOLUTION 0x3FF

//czas co który aktualizaowny jest moment - 10ms
#define TORQUE_DELAY 10

//maksymalne obroty przy których mo¿na zmieniæ kierunek
#define  MAX_CHANGE_DIR_SPEED 20


//œrodek 
#define CENTER ( ADC_RESOLUTION / 2 )

#define FWD 1
#define N 0
#define REV 2

//okres migania diod [ms]
#define LED_BLINKING_PERIOD 300

#endif //HARDDEF_H_INCLUDED

