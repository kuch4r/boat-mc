
#include <avr/io.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <util/atomic.h>
#include <util/crc16.h>
#include <stdio.h>
#include "main.h"
#include "CAN.h"
#include "EEPROM.h"
#include "uart.h"
#include "makra.h"


struct eeprom_struct EEMEM eeprom_data;
struct eeprom_struct non_volatile_data;

uint8_t eeprom_read(void)
{
	uint16_t crc = 0xFFFF;
	uint8_t *ptr = (uint8_t *) &non_volatile_data; //wskaŸnik na struktur¹ rzutujemy na zmienn¹ jednobajtow¹ ¿eby móc siê przesuwaæ co bajt przy liczeniu CRC
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		eeprom_read_block(&non_volatile_data, &eeprom_data, sizeof(struct eeprom_struct));
	}
	for(uint8_t i=0; i < EEPROM_READ_RETRIES ; i++){
		//liczymy crc z ca³ej struktury razem z crc które by³o zapisane, jeœli siê zgadza to wyliczone crc wyjdzie 0!
		for(uint8_t i=0; i < sizeof(non_volatile_data); i++){
			crc = _crc16_update(crc, *ptr++ );
		}
		//jeœli crc 0 to odczyt ok
		if(crc == 0){
			uart_puts_P("EEPROM read ok\n");
			return EEPROM_OK;
		}
		uart_puts_P("EEPROM read CRC missmatch - retring\n");
	}
	uart_puts_P("EEPROM read failed\n");
	return EEPROM_ERROR;
}

uint8_t eeprom_write(void)
{
	uint16_t crc = 0xFFFF;
	uint8_t *ptr = (uint8_t *) &non_volatile_data; //wskaŸnik na struktur¹ rzutujemy na zmienn¹ jednobajtow¹ ¿eby móc siê przesuwaæ co bajt przy liczeniu CRC
	
	//liczymy crc z ca³ej struktury bez dwóch ostatnich bajtów którymi jest suma crc
	for(uint8_t i=0; i < (sizeof(non_volatile_data)-2); i++){
		crc = _crc16_update(crc, *ptr++ );
	}
	//wpisujemy crc na ostatni¹ pozycjê
	non_volatile_data.crc = crc;
	
	for(uint8_t i=0; i < EEPROM_WRITE_RETRIES ; i++){
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			eeprom_write_block(&non_volatile_data, &eeprom_data, sizeof(struct eeprom_struct));
		}
		if (eeprom_read() == EEPROM_OK){ //sprawdzamy czy zapis siê powiód³
			uart_puts_P("EEPROM write ok\n");
			return EEPROM_OK;
		}
		uart_puts_P("EEPROM write CRC missmatch - retring\n");
	}
	uart_puts_P("EEPROM write failed\n");
	return EEPROM_ERROR;
	
}

void eeprom_restore_defaults(void){
	
	non_volatile_data.adc_board_position_max = ADC_BOARD_POSITION_MAX;
	non_volatile_data.adc_board_position_min = ADC_BOARD_POSITION_MIN;
	
	non_volatile_data.adc_current_offset = ADC_CURRENT_OFFSET;
	non_volatile_data.adc_current_scale = ADC_CURRENT_SCALE;
	
	//0x2003
	non_volatile_data.adc_voltage_offset = ADC_VOLTAGE_OFFSET;
	non_volatile_data.adc_voltage_scale = ADC_VOLTAGE_SCALE;
	//0x2004
	non_volatile_data.winch_overcurrent_value = OVERCURRENT_WINCH;
	non_volatile_data.board_overcurrent_value = OVERCURRENT_BOARD;
	//0x2005
	non_volatile_data.torque_init_val_winch_up = TORQUE_INIT_VAL_WINCH_UP;
	non_volatile_data.torque_init_val_winch_down = TORQUE_INIT_VAL_WINCH_DOWN;
	non_volatile_data.torque_init_val_board_up = TORQUE_INIT_VAL_BOARD_UP;
	non_volatile_data.torque_init_val_board_down = TORQUE_INIT_VAL_BOARD_DOWN;
	//0x2006
	non_volatile_data.torque_rising_speed_period_winch_up = TORQUE_RISING_SPEED_PERIOD_WINCH_UP;
	non_volatile_data.torque_rising_speed_period_winch_down = TORQUE_RISING_SPEED_PERIOD_WINCH_DOWN;
	non_volatile_data.torque_rising_speed_period_board_up = TORQUE_RISING_SPEED_PERIOD_BOARD_UP;
	non_volatile_data.torque_rising_speed_period_board_down = TORQUE_RISING_SPEED_PERIOD_BOARD_DOWN;
	//0x2007
	non_volatile_data.torque_rising_speed_winch_up = TORQUE_RISING_SPEED_PERIOD_WINCH_UP;
	non_volatile_data.torque_rising_speed_winch_down = TORQUE_RISING_SPEED_PERIOD_WINCH_DOWN;
	non_volatile_data.torque_rising_speed_board_up = TORQUE_RISING_SPEED_PERIOD_BOARD_UP;
	non_volatile_data.torque_rising_speed_board_down = TORQUE_RISING_SPEED_PERIOD_BOARD_DOWN;
}