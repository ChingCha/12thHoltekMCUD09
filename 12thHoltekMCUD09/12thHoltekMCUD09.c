#include "HT66F50.h"
#define ScanPort	_pd
#define ScanPortC	_pdc
#define ScanPort2	_pb
#define ScanPortC2	_pbc
#define DotPort		_pe
#define DotPortC	_pec

const unsigned short TAB[48] ={
  			0xFF,0x00,0x00,0xE7,0xE7,0x00,0x00,0xFF,//H
  			0xFF,0x81,0x00,0x3C,0x3C,0x00,0x81,0xFF,//O
  			0xFF,0x00,0x00,0x3F,0x3F,0x3F,0x3F,0xFF,//L
  			0xFF,0xFC,0xFC,0x00,0x00,0xFC,0xFC,0xFF,//T
  			0xFF,0x00,0x00,0x24,0x24,0x24,0x24,0xFF,//E
  			0xFF,0x00,0x00,0xE7,0xC3,0x99,0x3C,0xFF};//K
		
#pragma	vector ISR_TB0 @ 0x24
void PAUSE(unsigned short);
unsigned short ScanCode,Buffer[98];
unsigned short *ptr;
int x;
void main()
{	unsigned short i,temp;
	ScanPort=0; ScanPortC=0;
	ScanPort2=0; ScanPortC2=0;							//Config ScanPort as O/P
	DotPort=0; DotPortC=0;		
	x=0;												//Config DotPort as O/P
	_tbc=0b01000011;									//fTB=fSYS/4, TBCK=1
	ptr=Buffer; ScanCode=0b00000001;					//Initial Pointer and ScanCode
	_tb0e=1; _emi=1; 									//Enable TB0 Interrupt	
	for(i=0;i<96;i++)									//Clear Buffer[0]~Buffer[7]
	{	if(i>=48) *ptr++=0; else *ptr++=TAB[i];	   		//Copy Pattern to Buffer[8]~Buffer[15]
	}
 	while(1)
	{	ptr=Buffer;										//Initial Pointer			
		_tbon=1; PAUSE(30);								//Satrt TB0 & Delay 100ms					
		while(ptr!=Buffer);
		_tbon=0; DotPort=0;  							//Stop TB0
		temp=*ptr;										//Backup Buffer[0]
		for(i=0;i<47;i++) *ptr=*++ptr;					//Buffer Shift
		*ptr=temp;										//Reload temp to Buffer[15]
	}
}
void ISR_TB0()
{	
	int y =1;
	DotPort=0;		
	if(x==0)
	{
		ScanPort=ScanCode; 								//Turn-off Before New data
		ScanPort2=0b00000000;
	}else
	{
		ScanPort2=ScanCode; 							//Turn-off Before New data
		ScanPort=0b00000000;
		y=0;
	}					
	DotPort=*ptr++;										//Update Scan and Data
	_rl(&ScanCode);										//Up data Scan Code
	if(ScanCode==0b00000001)							//If all Lines been Scanned
		{					
			x=y;
			if(y==0)
			{
				ptr=Buffer;	
			}
											   			//Reset Pointer
		}
}
void PAUSE(unsigned short i)
{	unsigned short j;
	for(j=0;j<i;j++) _delay(10000);						//Dealy i*10ms
}	