
#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "CAN.h"


volatile uint8_t CAN_interrupt_flag=0;
volatile uint8_t CAN_SYNC_flag=0;
volatile uint8_t MOb_data[6][8];
volatile uint8_t device_id;

struct CAN_str CAN;


//Inicjalizacja CAN
void CAN_init(uint8_t dip_sw){
	uint16_t mob_0_id = SYNC_ID;		//SYNC
	uint16_t mob_1_id = 0x200 + BASE_ID; //RPDO 1
	uint16_t mob_2_id = 0x180 + BASE_ID; //TPDO 1
	uint16_t mob_3_id = 0x280 + BASE_ID; //TPDO 2
	uint16_t mob_4_id = 0x580 + BASE_ID; //SDO TX
	uint16_t mob_5_id = 0x600 + BASE_ID; //SDO RX
	
	//dodwania do adresu ustawienia z DIP switchy
	mob_1_id += dip_sw & 0x07;
	mob_2_id += dip_sw & 0x07;
	mob_3_id += dip_sw & 0x07;
	mob_4_id += dip_sw & 0x07;
	mob_5_id += dip_sw & 0x07;
	
	CANGCON = ( 1 << SWRES );   // Software reset
	CANTCON = 0x00;             // CAN timing prescaler set to 0
	
	CANBT1	|= 0x0E; //CAN Baud set 125kbit
	CANBT2	|= 0x0C; //CAN Baud set 125kbit
	CANBT3	|= 0x37; //CAN Baud set 125kbit
	
	for ( int8_t mob=0; mob<6; mob++ ) {
		CANPAGE = ( mob << 4 );   // Selects Message Object 0-5
		CANCDMOB = 0x00;          // Disable mob
		CANSTMOB = 0x00;          // Clear mob status register;
	}
			
	CANGIE	|= ( 1 << ENIT ) | (1<<ENRX); //Enable Receive Interrupt
	
	//MOb 0 configuration SYNC
	CANPAGE	 = (MOb_0<<4);
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = 0xFF; //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_0_id & 0x07) << 5);
	CANIDT1 = (mob_0_id >> 3);
	CANIE2	|= (1<<IEMOB0); //Enable Interrupt MOb 0
	
	//MOb 1 configuration odbierana paczka RPDO 1 0x200+
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
	
	//MOb 2 configuration nadawana paczka TPDO 1 0x180+
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
	
	
	//MOb 3 configuration nadawana paczka TPDO 2 0x280+
	CANPAGE	 = (MOb_3<<4);
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_3_id & 0x07) << 5);
	CANIDT1 = (mob_3_id >> 3);
	CANIE2	|= (1<<IEMOB3); //Enable Interrupt MOb 3
	
	//MOb 4 configuration nadawana paczka SDO TX
	CANPAGE	 = (MOb_4<<4);
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = (0x07 <<5); //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	//CANCDMOB |= TRANSMISSION;
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_4_id & 0x07) << 5);
	CANIDT1 = (mob_4_id >> 3);
	CANIE2	|= (1<<IEMOB4); //Enable Interrupt MOb 3
	
	//MOb 5 configuration odbierana paczka SDO RX
	CANPAGE	 = (MOb_5<<4);
	CANIDM4 = 0x00; //full mask setting
	CANIDM3 = 0x00; //full mask setting
	CANIDM2 = 0xFF; //full mask setting
	CANIDM1 = 0xFF; //full mask setting
	CANCDMOB = 0x00;
	CANCDMOB |= RECEPTION | ( 8 << DLC0);
	CANIDT4 = 0x00;
	CANIDT3 = 0x00;
	CANIDT2 = ((mob_5_id & 0x07) << 5);
	CANIDT1 = (mob_5_id >> 3);
	CANIE2	|= (1<<IEMOB5); //Enable Interrupt MOb 1
	
	
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

void CAN_task(){
	static uint8_t Update_data_flag=0;
	
	if(CAN_interrupt_flag !=0){	//Jeœli zg³oszone przerwanie
		CLEAR_CAN_interrupt_flag
		CANPAGE = CANHPMOB & 0xF0;      // Selects MOB with highest priority interrupt 
		
		
		if((CANPAGE>>4) == MOb_0){ //SYNC - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	//obiór SYNC-a
				SET_SYNC_flag;				//ustaw flagê ob³sugi synca
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION;		
		}
		
		else if((CANPAGE>>4) == MOb_1){ //RPDO 1 - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	//obiór SYNC-a
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){//przepisanie danych z rejestru do tablicy
					MOb_data[MOb_1][byte_nr] = CANMSG;
				}				
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION;
		}
		
		//czyszczenie flag przerwania dla paczek nadaj¹cych
		else if((CANPAGE>>4) == MOb_2){//TPDO 1
			CANSTMOB=0x00;
		}
		
		else if((CANPAGE>>4) == MOb_3){//TPDO 2
			CANSTMOB=0x00;
		}
		
		else if((CANPAGE>>4) == MOb_3){//SDO TX
			CANSTMOB=0x00;
		}
		
		else if((CANPAGE>>4) == MOb_5){ //SDO RX - odbiera
			if(CANSTMOB & ( 1 << RXOK)){	
				for(uint8_t byte_nr=0; byte_nr<8; byte_nr++ ){//przepisanie danych z rejestru do tablicy
					MOb_data[MOb_5][byte_nr] = CANMSG;
				}
			}
			CANSTMOB=0x00;
			CANCDMOB = 0x00;			//restart MOB-a
			CANCDMOB |= RECEPTION;
		}
	}
	else if(Update_data_flag){//mechanizm przepisywania danych PDO - po ka¿dym sync przepisywane s¹ œwie¿e dane
		static uint8_t frame=0;
		if(frame == MOb_1 ){ //RPDO 1
			//CAN.status_word = MOb_data[frame][1]<<8 | MOb_data[frame][0];
			frame++;
		}
		else if(frame == MOb_2 ){ //TPDO 1 - nadaje
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
		else if(frame == MOb_3 ){ //TPDO 2 nadaje
			MOb_data[frame][0] = CAN.state;
			MOb_data[frame][1] = CAN.water;
			MOb_data[frame][2] = CAN.water >>8;
			MOb_data[frame][3] = CAN.current_raw ;
			MOb_data[frame][4] = CAN.current_raw >> 8;
			MOb_data[frame][5] = CAN.current ;
			MOb_data[frame][6] = CAN.current >> 8;
			MOb_data[frame][7] = 0x00 | (uint8_t)(CAN.current > 256);
			CLEAR_update_data_flag;
			frame = 0;
		}						
	}
	else if( CAN_SYNC_flag  ){ //){ //Zleæ transmisjê po koleji transmisjê kolejnych paczek
		static uint8_t mob=MOb_2; //pierwszy MOb odbieraj¹cy
		CANPAGE = ( mob << 4 );						// Selects Message Object 0-5
		if((CANEN2 & ( 1 << mob )) == 0){		//Jeœli MOb jest wolny
			for(uint8_t byte_nr=0; byte_nr<8; byte_nr++  ){
				CANMSG = MOb_data[mob][byte_nr];
			}
			//czyszczenie rejestru statusu
			CANSTMOB = 0x00;
			//komenda nadania
			CANCDMOB = TRANSMISSION | ( 8 << DLC0);//zleæ transmisjê 8 bajtów
		}
		if(mob >= MOb_3 ){
			 mob=MOb_2;
			 CLEAR_SYNC_flag
			 SET_update_data_flag
		}		
		else mob++;
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
/*
void CAN_send_temp(uint16_t value, uint16_t value2){
	uint8_t mob=4;
	MOb_data[mob-1][0] = (value & 0x00FF);
	MOb_data[mob-1][1] = value >> 8;
	MOb_data[mob-1][3] = (value & 0x00FF);
	MOb_data[mob-1][4] = value >> 8;
	MOb_data[mob-1][5] = 0xFF
	MOb_data[mob-1][6] = 0xFF
	CANPAGE = ( mob << 4 );						// Selects Message Object 0-5
	if((CANEN2 & ( 1 << mob )) == 0){		//Jeœli MOb jest wolny
		for(uint8_t byte_nr=0; byte_nr<6; byte_nr++  ){
			CANMSG = MOb_data[mob-1][byte_nr];
		}
		CANSTMOB = 0x00;						//wyczyœæ status
		CANCDMOB = TRANSMISSION | ( 3 << DLC0);//zleæ transmisjê 8 bajtów
	}
}
*/
