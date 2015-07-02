
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "CAN.h"


volatile uint8_t CAN_interrupt_flag=0;
volatile uint8_t CAN_SYNC_flag=0;
volatile uint8_t CAN_HB_flag=0;
volatile uint8_t CAN_NMT_flag=0;
volatile uint8_t MOb_data[4][8];
volatile uint8_t device_id;

struct CAN_str CAN;


//Inicjalizacja CAN
void Init_CAN(uint8_t dip_sw){
	uint16_t mob_1_id = 0x200 + BASE_ID; //odbiera
	uint16_t mob_2_id = 0x180 + BASE_ID; //nadaje
	
	if(dip_sw & 0x01){
		mob_1_id += 1;
		mob_2_id += 1;
	}
	if(dip_sw & 0x02){
		mob_1_id += 2;
		mob_2_id += 2;
	}
	if(dip_sw & 0x04){
		mob_1_id += 4;
		mob_2_id += 4;
	}
	
	CANGCON = ( 1 << SWRES );   // Software reset
	CANTCON = 0x00;         // CAN timing prescaler set to 0
	
	CANBT1	|= 0x0E; //CAN Baud set 125kbit
	CANBT2	|= 0x0C; //CAN Baud set 125kbit
	CANBT3	|= 0x37; //CAN Baud set 125kbit
	
	for ( int8_t mob=0; mob<6; mob++ ) {
		CANPAGE = ( mob << 4 );        // Selects Message Object 0-5
		CANCDMOB = 0x00;             // Disable mob
		CANSTMOB = 0x00;           // Clear mob status register;
	}
			
	CANGIE	|= ( 1 << ENIT ) | (1<<ENRX); //Enable Receive Interrupt
	
	//MOb 0 configuration SYNC
	CANPAGE	 = (MOb_0<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	//CANIDM2 = 0x00; //full mask setting
	//CANIDM1 = 0x00; //full mask setting
	CANIDM2 = 0xFF; //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((SYNC_ID & 0x07) << 5);
	CANIDT1 = (SYNC_ID >> 3);
	CANIE2	|= (1<<IEMOB0); //Enable Interrupt MOb 0
	
	//MOb 1 configuration control message
	CANPAGE	 = (MOb_1<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = 0xFF; //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_1_id & 0x07) << 5);
	CANIDT1 = (mob_1_id >> 3);
	CANIE2	|= (1<<IEMOB1); //Enable Interrupt MOb 1
	
	/*
	//MOb 2 configuration
	CANPAGE	 = (MOb_2<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	//CANCDMOB |= RECEPTION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_2_ID & 0x07) << 5);
	CANIDT1 = (MOb_2_ID >> 3);
	CANIE2	|= (1<<IEMOB2); //Enable Interrupt MOb 2
	
	//MOb 3 configuration
	CANPAGE	 = (MOb_3<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_3_ID & 0x07) << 5);
	CANIDT1 = (MOb_3_ID >> 3);
	CANIE2	|= (1<<IEMOB3); //Enable Interrupt MOb 3
	*/
	
	//MOb 4 configuration
	CANPAGE	 = (MOb_2<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_2_id & 0x07) << 5);
	CANIDT1 = (mob_2_id >> 3);
	CANIE2	|= (1<<IEMOB2); //Enable Interrupt MOb 4
	
	/*
	//MOb 5 configuration
	CANPAGE	 = (MOb_5<<4);
	
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((MOb_5_ID & 0x07) << 5);
	CANIDT1 = (MOb_5_ID >> 3);
	CANIE2	|= (1<<IEMOB5); //Enable Interrupt MOb 5
	*/
	
	CLEAR_CAN_interrupt_flag
	
	CANGCON |= (1<<ENASTB); //Enable controller
}

//Obs³uga przerwania
SIGNAL ( CAN_INT_vect ){              // use interrupts
	
	SET_CAN_interrupt_flag

} 

/*ISR(BADISR_vect)
{
    SET_CAN_interrupt_flag
}*/

void CAN_Task(){
	static uint8_t Update_data_flag=0;
	
	if(CAN_interrupt_flag !=0){	//Jeœli zg³oszone przerwanie
		CLEAR_CAN_interrupt_flag
		CANPAGE = CANHPMOB & 0xF0;      // Selects MOB with highest priority interrupt 
		
		
		if((CANPAGE>>4) == MOb_0){ //sync - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	//obiór SYNC-a
				SET_SYNC_flag				//ustaw flagê ob³sugi synca
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION;		
		}
		
		
		/*else if((CANPAGE>>4) == MOb_1){ //Heartbeat - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	
				SET_HB_flag;
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;		
		}
		
		else if((CANPAGE>>4) == MOb_2){//paczka zwrotne z falownika - odbiera
			if(CANSTMOB & ( 1 << RXOK)){
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){
					MOb_data[0][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION | ( 8 << DLC0) ;
		}
		else if((CANPAGE>>4) == MOb_3){//paczka steruj¹ca falownikiem - nadaje
			CANSTMOB=0x00;
		}*/
		else if((CANPAGE>>4) == MOb_2){//paczka debuguj¹ca - nadaje
			CANSTMOB=0x00;
		}
		/*
		else if((CANPAGE>>4) == MOb_5){//NMT - nadaje
			CANSTMOB=0x00;
		}*/
	}
											
	else if(CAN_SYNC_flag){ //Zleæ transmisjê po koleji transmisjê kolejnych paczek
		static uint8_t mob=2;
		CANPAGE = ( mob << 4 );						// Selects Message Object 0-5
		if((CANEN2 & ( 1 << mob )) == 0){		//Jeœli MOb jest wolny
			for(uint8_t byte_nr=0; byte_nr<8; byte_nr++  ){
				CANMSG = MOb_data[mob-1][byte_nr];
			}
			CANSTMOB = 0x00;
			CANCDMOB = TRANSMISSION | ( 8 << DLC0);//zleæ transmisjê 8 bajtów
		}
		if(mob >= 2 ){
			 mob=2;
			 CLEAR_SYNC_flag
			 SET_update_data_flag
		}		
		else mob++;
	}
	else if(Update_data_flag){
		static uint8_t frame=0;
		if(frame == 0 ){ //paczka zwrotne z falownika - odbiera
			//CAN.status_word = MOb_data[frame][1]<<8 | MOb_data[frame][0];
			//CAN.torque = MOb_data[frame][5]<<8 | MOb_data[frame][4];
			//CAN.speed = MOb_data[frame][7]<<8 | MOb_data[frame][6];
			frame++;
		}
		else if(frame == 1 ){ //paczka wysy³ana - nadaje
			MOb_data[frame][0] = CAN.state;
			MOb_data[frame][1] = CAN.water;
			MOb_data[frame][2] = CAN.supply_voltage;
			MOb_data[frame][3] = CAN.current;
			MOb_data[frame][4] = CAN.diag_1;
			MOb_data[frame][5] = CAN.diag_2;
			MOb_data[frame][6] = 0x00;
			MOb_data[frame][7] = 0x00;
			frame++;
		}
		else if(frame == 2 ){ //paczka debuguj¹ca - nadaje
			MOb_data[frame][0] = CAN.state;
			MOb_data[frame][1] = 0x00;
			MOb_data[frame][2] = 0x00;
			MOb_data[frame][3] = 0x00;
			MOb_data[frame][4] = 0x00;
			MOb_data[frame][5] = 0x00;
			MOb_data[frame][6] = 0x00;
			MOb_data[frame][7] = 0x00;
			frame++;
		}
		else if(frame == 3 ){ //NMT - nadaje
			MOb_data[frame][0] = 0x01;
			MOb_data[frame][1] = 0;
			CLEAR_update_data_flag
			frame = 0;
		}
		
	}
}

void CAN_send(uint16_t value, uint8_t phase){
		uint8_t mob=2;
		MOb_data[mob-1][0] = (value & 0x00FF);
		MOb_data[mob-1][1] = value >> 8;
		MOb_data[mob-1][2] = phase;
		CANPAGE = ( mob << 4 );						// Selects Message Object 0-5
		if((CANEN2 & ( 1 << mob )) == 0){		//Jeœli MOb jest wolny
			for(uint8_t byte_nr=0; byte_nr<3; byte_nr++  ){
				CANMSG = MOb_data[mob-1][byte_nr];
			}
			CANSTMOB = 0x00;						//wyczyœæ status
			CANCDMOB = TRANSMISSION | ( 3 << DLC0);//zleæ transmisjê 8 bajtów
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