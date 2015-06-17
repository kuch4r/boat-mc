/* This is an ALWAYS automatically generated file based on the database ... */
/*It is recommended to change the database directly. If you want to prevent overwritten header files, make a copy of it and ensure you include the intended version in your node simulation.*/

#pragma once
#pragma pack(pop,8)

#ifndef MESSAGENAME_Paczka_nadawcza
#define MESSAGENAME_Paczka_nadawcza
typedef union 
{
	unsigned char      m_aucData[64];
	unsigned short int m_auwData[4];
	unsigned long  int m_aulData[2];

	struct
	{
	    unsigned int ain_1            : 16 ;
	    unsigned int                  :  8 ;
	    unsigned int Current          :  8 ;
	    unsigned int                  : 32 ;

	} __attribute__((__packed__));
}Paczka_nadawcza_;
typedef struct 
{
	unsigned int  m_unMsgID       ;
	unsigned char m_ucEXTENDED    ;
	unsigned char m_ucRTR         ;
	unsigned char m_ucDLC         ;
	unsigned char m_ucChannel	;
	Paczka_nadawcza_  m_sWhichBit     ;

	UINT m_ulTimeStamp;
	bool		     m_bCANFD        ;

} Paczka_nadawcza;
#endif /* MESSAGENAME_Paczka_nadawcza */
