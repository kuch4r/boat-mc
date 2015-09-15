//////////////////////////////////////////////////////////////////////////////////////////////////
// harddef.h - definicja sprzêtu dla programu PCF8591, p³ytka AVT3500
// 
// Autor: Rados³aw Koppel          Kompilator: WinAVR 20050214
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef HARDDEF_H_INCLUDED
#define HARDDEF_H_INCLUDED

//prêdkoœci
#define FCPU F_CPU
#define BAUD 14400


#define SPI_SCK_PORT B
#define SPI_SCK 1

#define SPI_MISO_PORT B
#define SPI_MISO 3

#define SPI_MOSI_PORT B
#define SPI_MOSI 2

#define SPI_CS_PORT B
#define SPI_CS 0

#define SPI_A0_PORT A
#define SPI_A0 7

#define LED_H_PORT B
#define LED_H 7

#define LCD_RESET_PORT G
#define LCD_RESET 0

#define ADC_6_PORT F
#define ADC_6 6

//wartoœæ napiêcie referencyjnego (dla ADC i DAC)
#define V_REF 4822 // mV


#define LED_PORT F
#define LED_1 4

//czas co który wywo³ywana jest g³ówna pêtla
#define MAIN_LOOP_PERIOD 58

//czas co który wywo³ywana jest g³ówna pêtla
#define DISPLAY_REFRESH_PERIOD 200

//czas co który wysy³any jest sync
#define SYNC_PERIOD 580

//czas co który wysy³any jest sync
#define SPI_TRANSMIT_TIME_OUT 20



#endif //HARDDEF_H_INCLUDED

