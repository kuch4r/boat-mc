

#include "SPLC501C.h"
#include "font5x7.h"
#include <avr/pgmspace.h>

#include "SPLC501C-AVR.h"
#include <util\delay.h>
#include <stdlib.h>

extern void GLCD_WriteCommand(unsigned char);
extern void GLCD_WriteData(unsigned char);
extern unsigned char GLCD_ReadData(void);
extern void GLCD_InitializePorts(void);

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void)
{

GLCD_InitializePorts();

GLCD_WriteCommand(SPLC501C_ADC_REVERSE);					
GLCD_WriteCommand(SPLC501C_COM0);	

GLCD_WriteCommand(SPLC501C_BIAS_19);						
GLCD_WriteCommand(SPLC501C_POWERON);						
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
_delay_us(50);
GLCD_WriteCommand(SPLC501C_VOLUME_MODE);						
GLCD_WriteCommand(SPLC501C_VOLUME_SET | 20);
GLCD_WriteCommand(0xA4);						
GLCD_WriteCommand(SPLC501C_DISPLAY_ON);						
GLCD_WriteCommand(SPLC501C_DISPLAY_NORMAL);		
GLCD_WriteCommand(SPLC501C_PAGE_ADDRESS | 0);
GLCD_WriteCommand(SPLC501C_COLUMN_ADDRESS_HI | 0);		
GLCD_WriteCommand(SPLC501C_COLUMN_ADDRESS_LO | 0);
GLCD_WriteCommand(SPLC501C_START_LINE | 0);	

}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_GoTo(unsigned char x, unsigned char y)
{
GLCD_WriteCommand(SPLC501C_COLUMN_ADDRESS_HI | (x >> 4));
GLCD_WriteCommand(SPLC501C_COLUMN_ADDRESS_LO | (x & 0x0F));
GLCD_WriteCommand(SPLC501C_PAGE_ADDRESS | y);
Tab_GoTo(x,y);
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_ClearScreen(void)
{
unsigned char x = 0, y = 0;
for(y = 0; y < (SCREEN_HEIGHT/PIXELS_PER_PAGE); y++)
	{
	GLCD_GoTo(0,y);
	for(x = 0; x < SCREEN_WIDTH; x++)
		{
		GLCD_WriteData(0);
		}
	}
}		
//-------------------------------------------------------------------------------------------------
// Function : GLCD_WriteChar
// Artuments : Char ASCII code
// Return value : none
//-------------------------------------------------------------------------------------------------
void GLCD_WriteChar(char charCode)
{
unsigned char fontCollumn;
for(fontCollumn = 0; fontCollumn < FONT_WIDTH; fontCollumn++)
  GLCD_WriteData(pgm_read_byte(&font5x7[((charCode- FONT_OFFSET) * FONT_WIDTH) + fontCollumn]));
GLCD_WriteData(0);
}
//-------------------------------------------------------------------------------------------------
// Function : GLCD_WriteString
// Arguments : pointer to null-terminated ASCII string
// Return value : none
//-------------------------------------------------------------------------------------------------
void GLCD_WriteString_progmem(const char * string)
{
	char bufor;
	while(bufor = pgm_read_byte(string++))
  {
  GLCD_WriteChar(bufor);
  }
}

void GLCD_WriteString(char * string)
{
while(*string)
  {
  GLCD_WriteChar(*string++);
  }
}
//-------------------------------------------------------------------------------------------------
// Function : GLCD_SetPixel
// Arguments : x-location, y-location, color (0 or 1)
// Return value : None
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(int x, int y, int color)
{
unsigned char temp = 0;
y= y%64;
x= x%132;  
GLCD_GoTo(x, (y/8)); 
temp = GLCD_ReadData(); 
if(color)
  temp |= (1 << (y % 8));
else
  temp &= ~(1 << (y % 8));
GLCD_GoTo(x, (y/8)); 
GLCD_WriteData(temp); 
}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char * bitmap,unsigned char left, unsigned char top, unsigned char width, unsigned char height)
{
unsigned char pageIndex, columnIndex;
for(pageIndex = 0; pageIndex < height / 8; pageIndex++)
  {
  GLCD_GoTo(left, top + pageIndex);
  for(columnIndex = 0; columnIndex < width; columnIndex++)
    GLCD_WriteData(*(bitmap++)); 
  }
}

void GLCD_voltage(int val)
{
	char bufor[3];
	char *str = bufor;
	
	itoa((int)(val/100), bufor, 10);
	if( val > 0 ) {
		GLCD_WriteChar(' ');
	}
	if( val < 1000 && val > -1000) {
		GLCD_WriteChar(' ');
	}
	while(*str){
		GLCD_WriteChar(*str++);
	}
	GLCD_WriteChar('.');
	if( val < 0 ) {
		val *= -1;
	}
	val = val%100;
	str = bufor;
	itoa((val), bufor, 10);
	if( val < 10 ){
		GLCD_WriteChar('0');
	}
	while(*str){
		GLCD_WriteChar(*str++);
	}	
}

//MAX 100 v/a w notacji [0,0625]
void GLCD_float(uint16_t val)
{
	char bufor[7]={0};
	char *str = bufor;
	itoa(val>>4, bufor, 10);
	//czêœæ dziesiêtna i kropka
	if((val>>4) < 10){
		GLCD_WriteChar('0');
		GLCD_WriteChar(*str++);
		GLCD_WriteChar('.');
	}
	else{
		GLCD_WriteChar(*str++);
		GLCD_WriteChar(*str++);
		GLCD_WriteChar('.');
	}
	//czesc za przecinkiem
	val &= 0x0000F;
	if(val==0){
		GLCD_WriteChar('0');
		GLCD_WriteChar('0');
	}
	else if(val==1){
		GLCD_WriteChar('0');
		GLCD_WriteChar('6');
	}
	else{
	val *= 1000;
	val /=16;
	str = bufor;
	itoa(val, bufor, 10);
		GLCD_WriteChar(*str++);
		GLCD_WriteChar(*str++);
	}
}

void GLCD_dec(int val){
	char bufor[7];
	GLCD_WriteString(itoa(val, bufor, 10));
}

void GLCD_clear(uint8_t clear){
	uint8_t i;
	for(i=0 ; i<clear ; i++)
		GLCD_WriteString(" ");
	GLCD_GoTo(GLCD_Get_x()-(clear*6), GLCD_Get_y());
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
