

#include "harddef.h"
#include "SPLC501C-AVR.h"
#include <avr/io.h>
#include "makra.h"
#include "delay.h"
#include <util\delay.h>


volatile unsigned char LCD[1056]={0};
//volatile unsigned char LCD[1]={0};
volatile uint16_t xs,ys;

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_InitializePorts(void)
{

	

	DDR(SPI_MOSI_PORT) |= (1<<SPI_MOSI);
	DDR(SPI_SCK_PORT) |= (1<<SPI_SCK);
	DDR(SPI_CS_PORT) |= (1<<SPI_CS);
	DDR(SPI_A0_PORT) |= (1<<SPI_A0);
	DDR(SPI_MISO_PORT) &= ~(1<<SPI_MISO);
	CS_HIGH;
	
	
	SPCR |= (1<<SPE) | (0<<DORD) |(1<<MSTR) | (1<<CPOL) |(1<<CPHA) | (1<<SPR1) |(0<<SPR0) | (0<<SPIE);
	
	
	
	//SPSR & _BV(SPIF);
	xs=0;
	ys=0;

}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(uint8_t dataToWrite)
{
	A0_HIGH;
	CS_LOW;
	//_delay_us(50);
	SPDR = dataToWrite;
	while((SPSR & _BV(SPIF)) == 0);
	CS_HIGH;
	LCD[(ys*132)+(xs)]=dataToWrite;
    if(xs<132)
        xs++;


}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(uint8_t commandToWrite)
{
	A0_LOW;
	CS_LOW;
	//_delay_us(50);
	SPDR = commandToWrite;
	while((SPSR & _BV(SPIF)) == 0);
	CS_HIGH;


}
//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
void Tab_GoTo(unsigned char x1, unsigned char y1)
{
    xs=x1;
    ys=y1;
}

unsigned char GLCD_ReadData(void)
{
    return LCD[(ys*132)+xs];
}

uint16_t GLCD_Get_x(void){
	return xs;
}

uint16_t GLCD_Get_y(void){
	return ys;
}
