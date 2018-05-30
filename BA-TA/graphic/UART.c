#include <avr/io.h>
#include <stdlib.h>
#include "UART.h"

#define XTAL 16000000


//Function to initialize UART.
//In our case with a BAUD of 38400, 8 databits and no parity
void InitUART(long choosenBaudRate, char choosenNumberOfDataBits, char choosenParity)
{
unsigned int tmp;

  if ((choosenBaudRate >= 110) && (choosenBaudRate <= 115200) && (choosenNumberOfDataBits >=5) && (choosenNumberOfDataBits <= 8))
  { 
    UCSR0A = 0b00100000;
    UCSR0B = 0b00011000;	
    UCSR0C = (choosenParity-5)<<1;

	if (choosenParity == 'E')
	{
		UCSR0C |= 0b00100000;
	}
	else if (choosenParity == 'O')	
	{
		UCSR0C |= 0b00110000;	
	}

    tmp = XTAL/(16*choosenBaudRate) - 1;
   
    UBRR0L = tmp;
    UBRR0H = tmp >> 8;
  }  
}

//Function to read char on port of the Arduino
char ReadChar()
{
  while ( (UCSR0A & (1<<7)) == 0 )
  {
	  //Repeat until we read something
  }                        
  return UDR0;
}

//Function to send a char to the Bluetooth module
void SendChar(char CharToSend)
{
  while ( (UCSR0A & (1<<5)) == 0 )
  {
	  //Repeat
  }
  UDR0 = CharToSend;
}

//Function taking use of the SendChar function, in order to easier send a whole string to the Bluetooth module
void SendString(char* StringToSend)
{
  //Repeat until whole string has been sent
  while (*StringToSend != 0)
  {
    SendChar(*StringToSend);
    StringToSend++;
  }
}

