
//struktura do przechowywania danych w eepromie
struct eeprom_struct {
	//0x2001
	uint16_t adc_board_position_max; 
	uint16_t adc_board_position_min;
	//0x2002
	int16_t adc_current_offset;
	uint16_t adc_current_scale;
	//0x2003
	int16_t adc_voltage_offset;
	uint16_t adc_voltage_scale;
	//0x2004
	uint8_t winch_overcurrent_value;
	uint8_t board_overcurrent_value;
	//0x2005
	uint16_t torque_init_val_winch_up;
	uint16_t torque_init_val_winch_down;
	uint16_t torque_init_val_board_up;
	uint16_t torque_init_val_board_down;
	//0x2006
	uint8_t torque_rising_speed_period_winch_up;
	uint8_t torque_rising_speed_period_winch_down;
	uint8_t torque_rising_speed_period_board_up;
	uint8_t torque_rising_speed_period_board_down;
	//0x2007
	uint16_t torque_rising_speed_winch_up;
	uint16_t torque_rising_speed_winch_down;
	uint16_t torque_rising_speed_board_up;
	uint16_t torque_rising_speed_board_down;
	
	//CRC
	uint16_t crc; //musi byæ ostatnie ¿eby crc siê liczy³o
};

uint8_t eeprom_read(void);
uint8_t eeprom_write(void);

void eeprom_restore_defaults(void);

#define EEPROM_READ_RETRIES 3
#define EEPROM_WRITE_RETRIES 2