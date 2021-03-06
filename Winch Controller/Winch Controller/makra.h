
#ifndef MAKRA_H_INCLUDED
#define MAKRA_H_INCLUDED

#include "harddef.h"
// Makra upraszczaj�ce dost�p do port�w
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

#define LED_0_ON PORT(LED_0_PORT) |= _BV(LED_0)
#define LED_0_OFF PORT(LED_0_PORT) &= ~(_BV(LED_0))

#define LED_1_ON PORT(LED_1_PORT) |= _BV(LED_1)
#define LED_1_OFF PORT(LED_1_PORT) &= ~(_BV(LED_1))

#define BUZZER_ON PORT(BUZZER_PORT) |= _BV(BUZZER)
#define BUZZER_OFF PORT(BUZZER_PORT) &= ~(_BV(BUZZER))

#define PWR_ON PORT(PWR_PORT) |= (1<<PWR)
#define PWR_OFF PORT(PWR_PORT) &= ~(1<<PWR)

#define WINCH_UP_CLEAR bit_is_clear(PIN(WINCH_UP_PORT),WINCH_UP)
#define WINCH_DOWN_CLEAR bit_is_clear(PIN(WINCH_DOWN_PORT),WINCH_DOWN)
#define WINCH_CLEAR bit_is_clear(PIN(AIN_2_PORT),AIN_2)

#define WINCH_UP_SET bit_is_set(PIN(WINCH_UP_PORT),WINCH_UP)
#define WINCH_DOWN_SET bit_is_set(PIN(WINCH_DOWN_PORT),WINCH_DOWN)
#define WINCH_SET bit_is_set(PIN(WINCH_SET_PIN_PORT),WINCH_SET_PIN)

#define WINCH_IS_ACTIVE WINCH_SET
#define BOARD_IS_ACTIVE WINCH_CLEAR

#define GET_TORQUE_RISING_SPEED ((winch_state == UP )?GET_TORQUE_RISING_SPEED_UP:GET_TORQUE_RISING_SPEED_DOWN)
#define GET_TORQUE_RISING_SPEED_UP (WINCH_IS_ACTIVE?non_volatile_data.torque_rising_speed_winch_up:non_volatile_data.torque_rising_speed_board_up)
#define GET_TORQUE_RISING_SPEED_DOWN (WINCH_IS_ACTIVE?non_volatile_data.torque_rising_speed_winch_down:non_volatile_data.torque_rising_speed_board_down)

#define GET_TORQUE_RISING_SPEED_PERIOD ((winch_state == UP )?GET_TORQUE_RISING_SPEED_PERIOD_UP:GET_TORQUE_RISING_SPEED_PERIOD_DOWN)
#define GET_TORQUE_RISING_SPEED_PERIOD_UP (WINCH_IS_ACTIVE?non_volatile_data.torque_rising_speed_period_winch_up:non_volatile_data.torque_rising_speed_period_board_up)
#define GET_TORQUE_RISING_SPEED_PERIOD_DOWN (WINCH_IS_ACTIVE?non_volatile_data.torque_rising_speed_period_winch_down:non_volatile_data.torque_rising_speed_period_board_down)

#define GET_TORQUE_INIT_VAL ((winch_state == UP )?GET_TORQUE_INIT_VAL_UP:GET_TORQUE_INIT_VAL_DOWN)
#define GET_TORQUE_INIT_VAL_UP (WINCH_IS_ACTIVE?non_volatile_data.torque_init_val_winch_up:non_volatile_data.torque_init_val_board_up)
#define GET_TORQUE_INIT_VAL_DOWN (WINCH_IS_ACTIVE?non_volatile_data.torque_init_val_winch_down:non_volatile_data.torque_init_val_board_down)


#define SW_1_SET bit_is_clear(PIN(SW_1_PORT),SW_1)
#define SW_2_SET bit_is_clear(PIN(SW_2_PORT),SW_2)
#define SW_3_SET bit_is_clear(PIN(SW_3_PORT),SW_3)
#define SW_4_SET bit_is_clear(PIN(SW_4_PORT),SW_4)
#define SW_5_SET bit_is_clear(PIN(SW_5_PORT),SW_5)
#define SW_6_SET bit_is_clear(PIN(SW_6_PORT),SW_6)
#define SW_7_SET bit_is_clear(PIN(SW_7_PORT),SW_7)
#define SW_8_SET bit_is_clear(PIN(SW_8_PORT),SW_8)

#define GET_DIP_SW_CAN_ADDR (Get_DIP_sw_word() & DIP_SW_CAN_ADDR_MASK)

#define SET_FLAG(target, flag)    target = target | (1 << flag)
#define CLEAR_FLAG( target, flag) target = target & (~(1 << flag))
#define HAS_FLAG( target, flag)   (target & (0x01 << flag))


//winch state
#define UP 2
#define DOWN 1
#define OFF 0
#define OVERLOAD 3

//winch status
#define INIT 0
#define INIT_DELAY 1
#define TORQUE_INCREASING 2
#define FULL_POWER 3

//power up source
#define WINCH_ON 0
#define SWITCH_ON 1

#define EEPROM_ERROR 1
#define EEPROM_OK 0

#define TRUE 1
#define FALSE 0

#endif //MAKRA_H_INCLUDED
