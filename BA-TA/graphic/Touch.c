/*
 * Touch.c
 *
 * Created: 03-05-2018 08:31:24
 *  Author: Soren
 */ 
#include "Touch.h" 
#include <avr/io.h>
#include <avr/cpufunc.h>
#define F_CPU 16000000
#include <util/delay.h>
// Data port definitions:
#define CLK_PORT PORTH 
#define CS_IRQ_PORT PORTE
#define IN_PORT PORTG
#define OUT_IRQ_OUTPORT DDRE

// Control port definitions:
#define CLK_BIT 3 
#define CS_BIT 3   
#define IN_BIT 5
#define OUT_BIT 5
#define IRQ_BIT 4

// LOCAL FUNCTIONS /
void Pulse()
{
	CLK_PORT |= 1 << CLK_BIT; // Clock high
	for(int i = 0; i<4;i++)
	{_NOP();}
	
	CLK_PORT &= ~(1 << CLK_BIT); // Clock low
	for(int i = 0; i<4;i++)
	{_NOP();}
}

// PUBLIC FUNCTIONS 
void TouchInit()
{
	
	DDRH |= (1<<CLK_BIT);
	DDRE |= (1<<CS_BIT);
	DDRG |= (1<<IN_BIT);
	DDRE &= ~(1<<OUT_BIT);
	DDRE &= ~(1<<IRQ_BIT);
	
	CS_IRQ_PORT |= 1 << CS_BIT;
	IN_PORT |= 1 << IN_BIT;
	CLK_PORT |= 1 << CLK_BIT;
	
}
int TouchRead(int xy)
{
	int cmd = 0;
	if (xy == 1)
	{
		cmd = 0x90; //read y
	}
	
	else if (xy == 2)
	{
		cmd = 0xD0; //read x
	}
	
	CS_IRQ_PORT &= ~(1 << CS_BIT);
	_NOP();
	_NOP();
	for(int i = 7; i >= 0; i--)
	{
		if ((cmd & 0b00000001<<i) != 0)
		{
			IN_PORT |= 1 << IN_BIT;
		}
		else
		{
			IN_PORT &= ~(1 << IN_BIT);
		}
		
		_NOP();
		_NOP();
		Pulse();
	}
	
	int data = 0;
	for(int i = 0; i < 12; i++)
	{
		data = data << 1;
		Pulse();
		if((PINE & 0b00100000) != 0)
		{
			data++;
		}
	}
	CS_IRQ_PORT |= 1 << CS_BIT;
	
	return data;
}


