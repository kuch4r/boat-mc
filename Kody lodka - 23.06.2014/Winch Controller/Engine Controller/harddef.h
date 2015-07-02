
#ifndef HARDDEF_H_INCLUDED
#define HARDDEF_H_INCLUDED

//czas co kt�ry wykonywana jest p�tla g�owna [4us]
#define MAIN_LOOP_PERIOD 25 //0,1ms


//Wej�cia
#define AIN_1_PORT B
#define AIN_1 4
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

#define LED_1_PORT D
#define LED_1 3


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

//Porty kierunku wyci�garki

#define WINCH_UP_PORT B
#define WINCH_UP 2

#define WINCH_DOWN_PORT C
#define WINCH_DOWN 4

//Podtrzymanie zasilania
#define PWR_PORT B
#define PWR 3

//Buzzer
#define BUZZER_PORT C
#define BUZZER 7

//pr�dko�� narastania momentu w promilach na TORQUE_RISING_SPEED_PERIOD
#define TORQUE_RISING_SPEED_WINCH_UP 4
#define TORQUE_RISING_SPEED_BOARD_UP 8
#define TORQUE_RISING_SPEED_WINCH_DOWN 4
#define TORQUE_RISING_SPEED_BOARD_DOWN 8


//czas co jaki zwi�kszana jest warto�� momnetu [ms]
#define TORQUE_RISING_SPEED_PERIOD_WINCH_UP 25
#define TORQUE_RISING_SPEED_PERIOD_BOARD_UP 25
#define TORQUE_RISING_SPEED_PERIOD_WINCH_DOWN 25
#define TORQUE_RISING_SPEED_PERIOD_BOARD_DOWN 25


//poczatkowa warto�� momentu w promilach
#define TORQUE_INIT_VAL	100
#define TORQUE_INIT_VAL_WINCH_UP 600
#define TORQUE_INIT_VAL_WINCH_DOWN 200
#define TORQUE_INIT_VAL_BOARD_UP 450
#define TORQUE_INIT_VAL_BOARD_DOWN 450

//czas kt�ry up�ywa od momntu w��czenia do za��czenia mocy - wyd�u�a czas �ycia przeka�nik�w [ms]
#define INITIAL_DELAY 15

//maksymalna wartos� pr�du wyci�graki [0,5A]
#define MAX_CURRENT_WINCH (64*2)

//maksymalna wartos� pr�du wyci�graki [0,5A]
#define MAX_CURRENT_BOARD (25*2)

//czas nieczu�o�ci [ms]
#define CURRENT_BLIND_TIME 75

//wsp saklujacy napi�cia
#define V_SCALE 170

//wsp offsetujacy napi�cia
#define V_OFFSET (135)

//wsp saklujacy pr�du
#define I_SCALE 800

//wsp offsetujacy pr�du
#define I_OFFSET (-511)

//wsp saklujacy wody
#define W_SCALE 200

//wsp offsetujacy wody
#define W_OFFSET (-25)

#define ADC_RESOLUTION 0x3FF

//czas do auto wy��czenia
#define TIME_TO_LIVE 50000

//okres migania diod [ms]
#define LED_BLINKING_PERIOD 300

//ilo�� kana��w ADC
#define ADC_CHANNELS_COUNT 11

//aktywne kana�y ADC
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
					
	

#endif //HARDDEF_H_INCLUDED

