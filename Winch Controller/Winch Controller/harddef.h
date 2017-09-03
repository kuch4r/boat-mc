
#ifndef HARDDEF_H_INCLUDED
#define HARDDEF_H_INCLUDED

//warning debug disables autopower off feature!!
//#define DEBUG

//console baud	
#define BAUD 57600


//czas co który wykonywana jest pêtla g³owna [4us]
#define MAIN_LOOP_PERIOD 25 //4us*25=0,1ms


//Wejœcia
#define AIN_1_PORT B
#define AIN_1 7
#define AIN_1_CHANNEL 4

#define AIN_2_PORT B
#define AIN_2 5
#define AIN_2_CHANNEL 6

#define AIN_3_PORT C
#define AIN_3 6
#define AIN_3_CHANNEL 10

#define AIN_4_PORT C
#define AIN_4 5
#define AIN_4_CHANNEL 9


//LEDy
#define LED_0_PORT D
#define LED_0 2

#define LED_1_PORT B
#define LED_1 4


//DIP SW
#define SW_1_PORT D
#define SW_1 7

#define SW_2_PORT D
#define SW_2 6

#define SW_3_PORT D
#define SW_3 5

#define SW_4_PORT B
#define SW_4 1

#define SW_5_PORT B
#define SW_5 0

#define SW_6_PORT C
#define SW_6 1

#define SW_7_PORT D
#define SW_7 1

#define SW_8_PORT C
#define SW_8 0

//Porty kierunku wyci¹garki

#define WINCH_UP_PORT B
#define WINCH_UP 2

#define WINCH_DOWN_PORT C
#define WINCH_DOWN 4

#define WINCH_SET_PIN_PORT B
#define WINCH_SET_PIN 5


//Podtrzymanie zasilania
#define PWR_PORT B
#define PWR 3

//Buzzer
#define BUZZER_PORT C
#define BUZZER 7

//prêdkoœæ narastania momentu w promilach na TORQUE_RISING_SPEED_PERIOD
#define TORQUE_RISING_SPEED_WINCH_UP 4
#define TORQUE_RISING_SPEED_BOARD_UP 8
#define TORQUE_RISING_SPEED_WINCH_DOWN 4
#define TORQUE_RISING_SPEED_BOARD_DOWN 8


//czas co jaki zwiêkszana jest wartoœæ momnetu [ms]
#define TORQUE_RISING_SPEED_PERIOD_WINCH_UP 25
#define TORQUE_RISING_SPEED_PERIOD_BOARD_UP 25
#define TORQUE_RISING_SPEED_PERIOD_WINCH_DOWN 25
#define TORQUE_RISING_SPEED_PERIOD_BOARD_DOWN 25


//poczatkowa wartoœæ momentu w promilach
#define TORQUE_INIT_VAL	100
#define TORQUE_INIT_VAL_WINCH_UP 600
#define TORQUE_INIT_VAL_WINCH_DOWN 200
#define TORQUE_INIT_VAL_BOARD_UP 450
#define TORQUE_INIT_VAL_BOARD_DOWN 450

//czas który up³ywa od momntu w³¹czenia do za³¹czenia mocy - wyd³u¿a czas ¿ycia przekaŸników [ms]
#define INITIAL_DELAY 15

//maksymalna wartosæ pr¹du wyci¹graki [0,5A]
#define OVERCURRENT_WINCH (66*2)

//maksymalna wartosæ pr¹du wyci¹graki [0,5A]
#define OVERCURRENT_BOARD (30*2)

//czas nieczu³oœci [ms]
#define CURRENT_BLIND_TIME 100

//wsp napiêcia
#define ADC_VOLTAGE_SCALE 170
#define ADC_VOLTAGE_OFFSET 135

//wsp pr¹du
#define ADC_CURRENT_SCALE 959
#define ADC_CURRENT_OFFSET (-480)

//wskazania miecza
#define ADC_BOARD_POSITION_MAX 100
#define ADC_BOARD_POSITION_MIN 0

#define ADC_BOARD_POSITION_SCALE (-138)
#define ADC_BOARD_POSITION_OFFSET 101


#define ADC_RESOLUTION 0x3FF

//czas do auto wy³¹czenia
#define TIME_TO_LIVE 50000

//okres migania diod [ms]
#define LED_BLINKING_PERIOD 300

//iloœæ kana³ów ADC
#define ADC_CHANNELS_COUNT 11

//aktywne kana³y ADC
#define ADC_ACTIVE_CHANNELS {0,\
0,\
0,\
0,\
1,\
0,\
0,\
0,\
0,\
1,\
1}			


//DIP SW can address mask
#define DIP_SW_CAN_ADDR_MASK 0x07
				
#define DIP_SW_IO_MASK 0x08



#endif //HARDDEF_H_INCLUDED

