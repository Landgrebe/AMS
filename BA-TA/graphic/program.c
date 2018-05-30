
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/io.h>
#include "TFTdriver.h"
#include "SystemMaster.h"
#include "Touch.h"
#include "UART.h"
#include "Bluetooth.h"
//#include "counting_millis.h"
long milliseconds_since = 0;
char intToDisplay[16];
int q = 0;


int main(void)
{
	// Initialize system
	
	DisplayInit();
	TouchInit();
	//CountingInit();
	//
	DisplayOn();
	welcome();
	//Time for Bluetooth module to load its info from its chip
	_delay_ms(1000);
	
	InitUART(38400, 8, 'N');
	
	//Time for bluetooth module to load its info from its chip -- SSP profile
	_delay_ms(500);
	
	InitBluetoothModule();

	
	_delay_ms(1000);
	
	
	
	// System startup
	makedisplay(0);
	//variables for touch
	int x,y;
	int idlestate = 0, busy = 0;
	int number = 1;
	int counting_background = 0;
	//check for input
	
	while(1)
	{
		
		x = TouchRead(1);
		y = TouchRead(2);
		_delay_ms(100);
	   counting_background ++;
	   if (x < 3700 && x > 3170 && y < 720 && y > 460)
			{
				int _state;
				busy = 0;
				if(idlestate == 0)
				{
					_state = makedisplay(number);
					if(number != 3)
					{
						number = 1;
						idlestate = 1;
						busy = 1;
					}	
				}
				
				else if(idlestate == 1 && busy == 0)
				{
					int succes = pickDevices(number, _state);
					if(succes == 1)
					{
						idlestate = 0;
						number = 1;
						_delay_ms(1000);
						makedisplay(0);	
					}
				}
				
			}

	   if (x < 2840 && x > 2260 && y < 720 && y > 460)
			{
				makedisplay(0);
				number = 1;
				idlestate = 0;
			}
		
		
		if (x < 1830 && x > 1540 && y < 720 && y > 460)
		{
			int up_return = 0;
			up_return = movePointerUp();
			if (up_return == 1)
			{
				number--;
			}
			
		}
		if (x < 1270 && x > 680 && y < 720 && y > 460)
		{
			int down_return = 0;
			down_return = movePointerDown();
			if (down_return == 1)
			{
				number++;
			}
		}
		if (counting_background == 5000/100) //5 sekunder tæller op ift delay
		{
			backgroundprocess(idlestate);
			counting_background = 0;
		}
		
	}
	
}
/*  */

