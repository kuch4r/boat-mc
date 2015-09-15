/*
 * IncFile1.h
 *
 * Created: 2014-09-06 22:28:00
 *  Author: LukaszKucharski
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#define BTN_1_PORT D
#define BTN_1 0

#define BTN_2_PORT D
#define BTN_2 1

#define BTN_3_PORT D
#define BTN_3 2

#define BTN_4_PORT D
#define BTN_4 3

#define BTN_5_PORT D
#define BTN_5 4


void Buttons_Init(void);
uint8_t Buttons_get_screen_line();
#endif /* INCFILE1_H_ */