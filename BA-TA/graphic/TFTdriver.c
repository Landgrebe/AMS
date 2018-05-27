/*
 * TFTdriver.c
 *
 * Created: 20-04-2018 9:41:31
 *  Author: Soren
 */ 
#include <avr/io.h>
#include <avr/cpufunc.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "character.h"
#include "TFTdriver.h"



// Data port definitions:
#define DATA_PORT_HIGH PORTA
#define DATA_PORT_LOW  PORTC

// Control port definitions:
#define WR_PORT PORTG
#define WR_BIT 2
#define DC_PORT PORTD
#define DC_BIT  7  //"DC" = RS
#define CS_PORT PORTG
#define CS_BIT  1
#define RST_PORT PORTG
#define RST_BIT 0
#define NULL 0


// Global Variable 
int start_x = 0;
int check_x = 0;
int backgoundred = 31, backgoundgreen = 63, backgoundblue = 31;


// LOCAL FUNCTIONS /
// ILI 9341 data sheet, page 238
void WriteCommand(unsigned int command)
{
	DATA_PORT_LOW = command;
	
	DC_PORT &= ~(1<<DC_BIT);
	
	CS_PORT &= ~(1<<CS_BIT);
	
	WR_PORT &= ~(1<<WR_BIT);
	
	_NOP();
	
	WR_PORT |= (1<<WR_BIT);
	
	_NOP();
}

// ILI 9341 data sheet, page 238
void WriteData(unsigned int data)
{
	DATA_PORT_HIGH = data >> 8;
	DATA_PORT_LOW = data;
	
	DC_PORT |= (1<<DC_BIT);
	
	CS_PORT &= ~(1<<CS_BIT);
	
	WR_PORT &= ~(1<<WR_BIT);
	
	_NOP();
	
	WR_PORT |= (1<<WR_BIT);
	
	_NOP();
}

void letterhelper(int numberletter, int startx, int starty,unsigned char Red, unsigned char Green, unsigned char Blue)
{
	if (startx != check_x) //secure we don't overwrite the last command.
	{
		check_x = startx;
		start_x = startx;
	}
	
	numberletter = numberletter*2; //first parameter is length second is the offset
	int count = array_carrier[numberletter+3]-array_carrier[numberletter+1]; //find the number of interger
	unsigned int offset = array_carrier[numberletter+1]; //set the offset compared to the array
	int fixed_letter = (array_carrier[numberletter]/8); //find the length of the sign
	int modulus = array_carrier[numberletter]%8;
	if (modulus>0) //getting the Rest with modulus % operator
	{
		fixed_letter++; //add one length
	}
	drawXletter(bogstaver,fixed_letter,count,start_x,starty, offset,modulus,Red,Green,Blue);
	start_x = start_x+array_carrier[numberletter]+1; //sets the new startpoint for next session
}



// PUBLIC FUNCTIONS 

// Initializes (resets) the display
// Initializes (resets) the display
void DisplayInit()
{
	DDRG |= 0b00000111;
	DDRD |= 0b10000000;
	
	DDRA = 0xFF;
	DDRC = 0xFF;
	
	PORTG |= 0b00000111;
	PORTD |= 0b10000000;
	
	RST_PORT &= ~(1<<RST_BIT);
	
	_delay_ms(500);
	
	RST_PORT |= (1<<RST_BIT);
	_delay_ms(130);
	
	SleepOut();
	
	DisplayOn();
	
	MemoryAccessControl(0b00001000);
	
	InterfacePixelFormat(0b00000101);
}

void DisplayOff()
{
	WriteCommand(0x28);
}

void DisplayOn()
{
	WriteCommand(0x29);
}

void SleepOut()
{
	WriteCommand(0x11);
}

void MemoryAccessControl(unsigned char parameter)
{
	WriteCommand(0x36);
	WriteData(parameter);
}

void InterfacePixelFormat(unsigned char parameter)
{
	WriteCommand(0x3A);
	WriteData(parameter);
}

void MemoryWrite()
{
	WriteCommand(0x2C);
}

// Red 0-31, Green 0-63, Blue 0-31
void WritePixel(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	WriteData((Red<<11) | (Green<<5) | Blue);
}

// Set Column Address (0-239), Start > End
void SetColumnAddress(unsigned int Start, unsigned int End)
{
	WriteCommand(0x2A);
	WriteData(Start>>8);
	WriteData(Start);
	WriteData(End>>8);
	WriteData(End);
}

// Set Page Address (0-319), Start > End
void SetPageAddress(unsigned int Start, unsigned int End)
{
	WriteCommand(0x2B);
	WriteData(Start>>8);
	WriteData(Start);
	WriteData(End>>8);
	WriteData(End);
}

void FillRectangle(unsigned int StartX, unsigned int StartY, unsigned int Width, unsigned int Height, unsigned char Red, unsigned char Green, unsigned char Blue)
{
	SetPageAddress(StartX,StartX+Width);
	SetColumnAddress(StartY,StartY+Height);
	MemoryWrite();
	for(unsigned long i = 0; i<((unsigned long)Width*Height); i++)
	{
		WritePixel(Red,Green,Blue);	
	}
	
}

void drawletters(char str[],int startx, int starty,unsigned char Red, unsigned char Green, unsigned char Blue) 
{
	int test[25]={},i=0,j;
	
	while(str[i]!='\0') {test[i]=str[i]; i++;}
	
	for(j=0;j<i;j++)
	{
		int not_space = test[j]-33; //convert to array from ascii
		if (not_space != -1)
		{letterhelper(not_space,startx,starty,Red,Green,Blue);}
		else
		{start_x = start_x+10;}
	}
	start_x = startx;
	return 0;
}

void drawNumber(long int number,int startx, int starty,unsigned char Red, unsigned char Green, unsigned char Blue)
{	
	int zero_check = 0;
	int num[5] = {number % 10,(number/10) % 10,(number/100) % 10,(number/1000) % 10,(number/10000) % 10};//check every digit
	for (int i = 4; i>=0; i--) //start by the biggest digit
	{
		//letterhelper(num[i]+15,startx,starty);
		if (num[i] != 0 | zero_check == 1)
		{
			letterhelper(num[i]+15,startx,starty,Red,Green,Blue); //add 15 to get to the right number in array
			zero_check = 1;
		}
		
	}
	start_x = startx;
	
	
}

void setBaggroundPixel(int red, int green, int blue)
{
	backgoundred = red;
	backgoundgreen = green;
	backgoundblue = blue;
}

void drawXletter(const uint8_t bitmap[],int length,int count,int startx,int starty, int letter, int modulus,unsigned char Red, unsigned char Green, unsigned char Blue)
{
	int startpage = startx; //set startpage
	int stoppage= startx+7; //set stoppage
	int colum = starty; //set colum
	for (int g=0;g<count;g++)
	{
		SetPageAddress(startpage,stoppage);
		SetColumnAddress(colum,colum);
		MemoryWrite();
		for (int i=0; i<8; i++)
		{
			if((bitmap[g+letter] & (0b10000000>>i)) != 0) //check for bit in array
			{WritePixel(Red,Green,Blue);}
			else
			{WritePixel(backgoundred, backgoundgreen, backgoundblue);}
		}
		startpage = stoppage+1; //set new startpage +1 for not writing to the same bit.
		stoppage = stoppage+8; //set new stoppage +7 for ending the byte, +1 because off startpage.
		
		if(stoppage >= length*8+startx) //check length of character
		{
			colum++; // next colum
			startpage = startx; //reset startpage
			stoppage = startpage+7; //reset stoppage
		}
	}
	//delete old character on display
	colum = starty; //reset colum
	
	for (int i=0; i<count/length;i++)
	{
		if(modulus == 0)
		{modulus = 8;}
		SetPageAddress(startx+(length*8-(8-modulus)),startx+(length*8-(8-modulus))+7); //set white, where the character stopped
		SetColumnAddress(colum,colum);
		MemoryWrite();
		for (int j=0; j<8; j++)
		{WritePixel(backgoundred, backgoundgreen, backgoundblue);}
		colum++;
	}
	
}

//not implementet, for future use
/*
void drawBitmap(const uint8_t bitmap[],int size, int x, int y) {
	int startpage = y;
	int stoppage= startpage+8;
	int count = 0;
	int size2 = sizeof(bitmap)/sizeof(bitmap[0]);
	int colum = x;
	int colum_check = 0;
	for (long int g=0;g<size;g++)
	{
		
		SetPageAddress(startpage,stoppage);
		SetColumnAddress(colum+colum_check,colum+colum_check);
		MemoryWrite();
		for (int i=0; i<8; i++)
		{
			
			if((bitmap[g] & (0b10000000>>i)) != 0)
			{
				WritePixel(31,0,0);
			}
			else
			{
				WritePixel(31,63,31);
			}
		}
		colum_check = colum_check+1;
		
		if(colum_check > 8)
		{
			count++;
			colum_check=0;
			startpage = y+(8*count)-1;
			stoppage = startpage+(8*count);
		}
		
		
	}
	
}
*/