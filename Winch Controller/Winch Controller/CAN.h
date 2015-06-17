/*
 * CAN.h
 *
 * Created: 2013-02-03 14:16:41
 *  Author: LukaszKucharski
 */ 

//CAN id 0x03

#ifndef CAN_H_
#define CAN_H_

void CAN_init(uint8_t);
void CAN_task();
uint8_t CAN_Heart_Beat_received(void);
void Set_NMT_flag(void);
void Clear_NMT_flag(void);
#define RECEPTION 1<<CONMOB1
#define TRANSMISSION 1<<CONMOB0

#define SET_CAN_interrupt_flag CAN_interrupt_flag=1;
#define CLEAR_CAN_interrupt_flag CAN_interrupt_flag=0;

#define SET_SYNC_flag CAN_SYNC_flag=1;
#define CLEAR_SYNC_flag CAN_SYNC_flag=0;

#define SET_HB_flag CAN_HB_flag=1;
#define CLEAR_HB_flag CAN_HB_flag=0;


#define SET_update_data_flag Update_data_flag=1;
#define CLEAR_update_data_flag Update_data_flag=0;

#define MOb_0 0
#define MOb_1 1
#define MOb_2 2
#define MOb_3 3
#define MOb_4 4
#define MOb_5 5

//Sync - odbiera
#define SYNC_ID 0x080
//id bazowe - mo¿na zwiêkszaæ DIP SW
#define BASE_ID 0x10

struct CAN_str {
	uint8_t current;
	int8_t water;
	int8_t supply_voltage;
	uint8_t state;
	uint8_t diag_1;
	uint8_t diag_2;	
	
	int16_t water_raw;
	int16_t current_raw;
};



#endif /* CAN_H_ */
