/*
 * CAN.h
 *
 * Created: 2013-02-03 14:16:41
 *  Author: LukaszKucharski
 */ 

//CAN id 0x03

#ifndef CAN_H_
#define CAN_H_

void CAN_Init();
void CAN_Task();
uint8_t CAN_Heart_Beat_received(void);
void Set_NMT_flag(void);
void Clear_NMT_flag(void);
void CAN_Send_Sync();
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

#define BMS_TPDO_1 0
#define BMS_TPDO_2 1
#define BMS_TPDO_3 2



//Sync - odbiera
#define MOb_0_ID 0x080
//BMS_TPDO_1 - odbiera
#define MOb_1_ID 0x186
//BMS_TPDO_2 - odbiera
#define MOb_2_ID 0x206
//BMS_TPDO_3 - odbiera
#define MOb_3_ID 0x286

//paczka debuguj¹ca - nadaje
#define MOb_4_ID 0x187
//NMT - nadaje
#define MOb_5_ID 0x000

#define INV_ID 0x01

struct CAN_str {
	uint16_t BMS_voltage;
	int16_t BMS_current;
	uint8_t BMS_SOC;
	uint16_t BMS_remaining_capacity;
	uint8_t BMS_state;
	uint16_t BMS_V_min;
	uint8_t BMS_V_min_nr;
	uint16_t BMS_V_max;
	uint8_t BMS_V_max_nr;
	uint16_t BMS_V_avrg;
	uint8_t BMS_T_min;
	uint8_t BMS_T_min_nr;
	uint8_t BMS_T_max;
	uint8_t BMS_T_max_nr;
	uint8_t BMC_DMCC_p;
	
	uint16_t status_word;
	int16_t torque;
	int32_t speed;
	int16_t actual_torque;
	int32_t motor_speed;
	uint8_t state;
	uint8_t diag_1;
	uint8_t diag_2;	
};



#endif /* CAN_H_ */
