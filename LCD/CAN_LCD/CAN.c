
#include <avr/io.h>
#include <avr/interrupt.h>
#include "CAN.h"
#include "harddef.h"
#include "timer.h"


volatile uint8_t CAN_interrupt_flag=0;
volatile uint8_t CAN_SYNC_flag=0;
volatile uint8_t CAN_HB_flag=0;
volatile uint8_t CAN_NMT_flag=0;
volatile uint8_t MOb_data[5][8];

struct CAN_str CAN;


//Inicjalizacja CAN
void CAN_Init(){
	
	CANGCON = ( 1 << SWRES );   // Software reset
	CANTCON = 0x00;         // CAN timing prescaler set to 0
	
	CANBT1	|= 0x0A; //CAN Baud set 125kbit
	CANBT2	|= 0x0C; //CAN Baud set 125kbit
	CANBT3	|= 0x37; //CAN Baud set 125kbit
	
	for ( int8_t mob=0; mob<14; mob++ ) {
		CANPAGE = ( mob << 4 );        // Selects Message Object 0-5
		CANCDMOB = 0x00;             // Disable mob
		CANSTMOB = 0x00;           // Clear mob status register;
	}
			
	CANGIE	|= ( 1 << ENIT ) | (1<<ENRX); //Enable Receive Interrupt
	
	//MOb 0 configuration SYNC
	CANPAGE	 = (MOb_0<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_0_ID & 0x07) << 5);
	CANIDT1 = (MOb_0_ID >> 3);
	CANIE2	|= (1<<IEMOB0); //Enable Interrupt MOb 3
	
	//MOb 1 configuration BMS_TPDO_1
	CANPAGE	 = (MOb_1<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = 0xFF; //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_1_ID & 0x07) << 5);
	CANIDT1 = (MOb_1_ID >> 3);
	CANIE2	|= (1<<IEMOB1); //Enable Interrupt MOb 1
	
	//MOb 2 configuration BMS_TPDO_2
	CANPAGE	 = (MOb_2<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_2_ID & 0x07) << 5);
	CANIDT1 = (MOb_2_ID >> 3);
	CANIE2	|= (1<<IEMOB2); //Enable Interrupt MOb 2
	
	//MOb 3 configuration BMS_TPDO_3
	CANPAGE	 = (MOb_3<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_3_ID & 0x07) << 5);
	CANIDT1 = (MOb_3_ID >> 3);
	CANIE2	|= (1<<IEMOB3); //Enable Interrupt MOb 3
	
	//MOb 4 configuration
	CANPAGE	 = (MOb_4<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_4_ID & 0x07) << 5);
	CANIDT1 = (MOb_4_ID >> 3);
	CANIE2	|= (1<<IEMOB4); //Enable Interrupt MOb 4
	
	//MOb 5 configuration INV_TPDO_3
	CANPAGE	 = (MOb_5<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_5_ID & 0x07) << 5);
	CANIDT1 = (MOb_5_ID >> 3);
	CANIE2	|= (1<<IEMOB5); //Enable Interrupt MOb 5
	
	CLEAR_CAN_interrupt_flag
	
	CANGCON |= (1<<ENASTB); //Enable controller
}

//Obs³uga przerwania
SIGNAL ( CANIT_vect ){              // use interrupts
	
	SET_CAN_interrupt_flag

} 

/*ISR(BADISR_vect)
{
    SET_CAN_interrupt_flag
}*/

void CAN_Task(){
	static uint8_t Update_data_flag=0;
	static uint16_t sync_timer=0;
	
	//obs³uga nadawania sync-a
	if(timer_time_elapsed(sync_timer) > SYNC_PERIOD){ //obs³uga nadawania sync-a
		sync_timer=timer_get();
		CANPAGE = MOb_0 & 0xF0;   //wybierz mob
		CANSTMOB=0x00;
		CANCDMOB = TRANSMISSION | ( 0 << DLC0); //zleæ transmisjê
		SET_update_data_flag
	}
	
	//obs³uga pozosta³y paczek
	if(CAN_interrupt_flag !=0){	//Jeœli zg³oszone przerwanie
		CLEAR_CAN_interrupt_flag
		CANPAGE = CANHPMOB & 0xF0;      // Selects MOB with highest priority interrupt 
	
		
		if((CANPAGE>>4) == MOb_1){ //BMS_TPDO_1 - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[BMS_TPDO_1][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;		
		}
		
		else if((CANPAGE>>4) == MOb_2){//BMS_TPDO_2 - odbiera
			if(CANSTMOB & ( 1 << RXOK)){
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[BMS_TPDO_2][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;
		}
		else if((CANPAGE>>4) == MOb_3){////BMS_TPDO_3 - odbiera
			if(CANSTMOB & ( 1 << RXOK)){
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[BMS_TPDO_3][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;
		}
		else if((CANPAGE>>4) == MOb_4){//paczka ok kontrolera wyci¹garki
			if(CANSTMOB & ( 1 << RXOK)){
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[WINCH_CTRL_TPDO_1][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;
		}
				
		else if((CANPAGE>>4) == MOb_5){//paczka od falownika TPDO_3
			if(CANSTMOB & ( 1 << RXOK)){
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[INV_TPDO_3][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;
		}
		
	}
											
	/*else if(CAN_SYNC_flag){ //Zleæ transmisjê po koleji transmisjê kolejnych paczek
		static uint8_t mob=3;
		CANPAGE = ( mob << 4 );						// Selects Message Object 0-5
		if((CANEN2 & ( 1 << mob )) == 0){		//Jeœli MOb jest wolny
			for(uint8_t byte_nr=0; byte_nr<8; byte_nr++  ){
				CANMSG = MOb_data[mob-2][byte_nr];
			}
			CANSTMOB = 0x00;
			if(mob==5){//jeœli NMT
				CANCDMOB = TRANSMISSION | ( 2 << DLC0);//zleæ transmisjê 8 bajtów	
			}							//wyczyœæ status
			else{
				CANCDMOB = TRANSMISSION | ( 8 << DLC0);//zleæ transmisjê 8 bajtów
			}
		}
		if((mob >= 4 && !CAN_NMT_flag) || (mob >= 5 && CAN_NMT_flag)){
			 mob=2;
			 CLEAR_SYNC_flag
			 SET_update_data_flag
		}		
		else mob++;
	}
	*/
	else if(Update_data_flag){
		static uint8_t frame = 0;
		if(frame == BMS_TPDO_1 ){ // - odbiera
			CAN.BMS_current = MOb_data[frame][1]<<8 | MOb_data[frame][0];
			CAN.BMS_voltage = MOb_data[frame][3]<<8 | MOb_data[frame][2];
			CAN.BMS_SOC = MOb_data[frame][4];
			CAN.BMS_remaining_capacity = MOb_data[frame][6]<<8 | MOb_data[frame][5];
			CAN.BMS_state = MOb_data[frame][7];
			frame++;
		}
		else if(frame == BMS_TPDO_2 ){ // - odbiera
			CAN.BMS_V_min = MOb_data[frame][1]<<8 | MOb_data[frame][0];
			CAN.BMS_V_min_nr = MOb_data[frame][2];
			CAN.BMS_V_max = MOb_data[frame][4]<<8 | MOb_data[frame][3];
			CAN.BMS_V_max_nr = MOb_data[frame][5];
			CAN.BMS_V_avrg = MOb_data[frame][7]<<8 | MOb_data[frame][6];
			frame++;
		}
		else if(frame == BMS_TPDO_3 ){ //odbiera
			CAN.BMS_T_min = MOb_data[frame][0];
			CAN.BMS_T_min_nr = MOb_data[frame][1];
			CAN.BMS_T_max =  MOb_data[frame][2];
			CAN.BMS_T_max_nr = MOb_data[frame][3];
			CAN.BMC_DMCC_p = MOb_data[frame][7];
			frame++;
		}
		else if(frame == WINCH_CTRL_TPDO_1 ){ //
			CAN.WINCH_CTRL_water_level = MOb_data[frame][1];
			CAN.WINCH_CTRL_supply_voltage = MOb_data[frame][2];
			frame++;
		}
		else if(frame == INV_TPDO_3 ){ //
			CAN.INV_RPMs = MOb_data[frame][7]<<24 | MOb_data[frame][6]<<16 | MOb_data[frame][5]<<8 | MOb_data[frame][4];
			CAN.INV_torque = MOb_data[frame][3]<<8 | MOb_data[frame][2];
			frame++;
		}
		else if(frame > INV_TPDO_3) { //NMT - nadaje
			//MOb_data[frame][0] = 0x01;
			//MOb_data[frame][1] = INV_ID;
			CLEAR_update_data_flag
			frame = 0;
		}
		
	}
}


uint8_t	CAN_Heart_Beat_received(void){
	return CAN_HB_flag;
}

void Set_NMT_flag(void){
	CAN_NMT_flag=1;
}

void Clear_NMT_flag(void){
	CAN_NMT_flag=0;
}