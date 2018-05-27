/*
 * display.c
 *
 * Created: 10-05-2018 14:39:37
 *  Author: Soren
 */ 
#include <stdbool.h>
#include <avr/io.h>
#include <avr/cpufunc.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <string.h>
#include "TFTdriver.h"
#include "display.h"
#include "Bluetooth.h"
#include "logo.h"
char* get_name[3] = {0,0,0}; 
char* get_address[3] = {0,0,0};


char* variable1;
char* variable2;

char save_name[3][40]; 
char save_address[3][40]; 

// Global Variable 
int start_position_x = 50;
int start_position_y = 70;
int pointer_position_y = 0;
int locked = 1;
int pointer_max = 0;
int devices = 0;



// PUBLIC FUNCTIONS 
void wait()
{
	cleanDisplay();
	drawletters("SEARCHING", 100,108,0,0,0);
	MakeLogo();
	
}
void welcome()
{
	cleanDisplay();
	drawletters("WELCOME", 100,108,0,0,0);
	MakeLogo();
}
int makedisplay(int state)
{
	
	switch (state)
		{
		
			case 1: //add device
				wait();
				trustedDevicesScan();
				for (int i=0; i<4; i++)
				{
					variable2 = getTrustedDeviceAddress(i+1);
					get_address[i] = malloc(strlen(variable2)+1); //0 terminering
					strcpy(get_address[i],variable2);
				}
				
				for (int i=0; i<4; i++)
				{
					variable1 = getTrustedDeviceName(i+1);
					get_name[i] = malloc(strlen(variable1)+1); //0 terminering
					strcpy(get_name[i],variable1);
				}
				cleanDisplay();
				drawletters("AVALIBLE DEVICES:", start_position_x-40,start_position_y-40,0,0,0);
				devices = 0;
				
				for (int i=0;i<4;i++)
				{
					
					if (strcmp(save_address[i],"N/A") !=  0)
					{
						drawletters(get_name[i], start_position_x,start_position_y+(30*i),0,0,0);
						devices++;
					}
				}
				
				//drawNumber(devices,300,100,0,0,0); //debug devices
				addPointer();
				lock();
				TouchDraw();
				pointer_max = devices;
				devices = 0;
				break;
			case 2: //remove device
				cleanDisplay();
				pointer_position_y = 0;
				
				drawletters("REMOVE DEVICE:", start_position_x-40,start_position_y-40,0,0,0);
				pointer_max = devices;
			
				for (int i=0;i<devices;i++)
					{
						drawletters(save_name[i], start_position_x,start_position_y+(30*i),0,0,0);	
					}
			
				//drawNumber(devices,300,100,0,0,0); //debug devices
				if(devices > 0)
				{
					addPointer();
				}
				lock();
				TouchDraw();
				break;
			case 3: //lock switch
				toggleLock();
				toggleSwitch();
				
				break;
			default:
				cleanDisplay();
				pointer_max = 3;
				drawletters("ADD DEVICE", start_position_x,start_position_y,0,0,0);
				drawletters("REMOVE DEVICE", start_position_x,start_position_y+30,0,0,0);
				toggleSwitch();
				addPointer();
				lock();
				TouchDraw();
				MakeLogo();
				pointer_position_y = 0;
				break;
		}
		return state;
}
int movePointerUp()
{
	if (pointer_position_y > 0)
	{
		drawletters(">", start_position_x-20,start_position_y+(pointer_position_y*30),31,63,31);
		pointer_position_y--;
		drawletters(">", start_position_x-20,start_position_y+(pointer_position_y*30),0,0,0);
		return 1;
	}
	else
	{return 0;}
}
int movePointerDown()
{
	if (pointer_position_y < pointer_max-1)
	{
		
		drawletters(">", start_position_x-20,start_position_y+(pointer_position_y*30),31,63,31);
		pointer_position_y++;
		drawletters(">", start_position_x-20,start_position_y+(pointer_position_y*30),0,0,0);
		return 1;
	}
	else
	{return 0;}
	
}

// LOCAL FUNCTIONS /

void addPointer()
{
		drawletters(">", start_position_x-20,start_position_y,0,0,0);
}
void lock()
{
	
	if (locked == 1)
	{
		FillRectangle(261,0,60,60,0,63,0);
	}
	else if (locked == 0)
	{
		FillRectangle(261,0,60,60,31,0,0);
	}
}
void toggleLock()
{
	locked++;
	if (locked == 3)
	{locked = 0;}
	lock();
	
}


int pickDevices(int num, int _state)
{
	num--;
	if (_state == 1)
	{
		strcpy(save_name[devices], get_name[num]);
		strcpy(save_address[devices], get_address[num]);
		
		devices++;
        cleanDisplay();
		drawletters("You picked:", 50 ,100,0,0,0);
		drawletters(get_name[num], 50 ,140,0,0,0);
		return 1;
	}
	else if(_state == 2)
	{
		if (devices > 0)
		{
			cleanDisplay();
			drawletters("You deleted:", 50 ,100,0,0,0);
			drawletters(save_name[num], 50 ,140,0,0,0);
			for (int i=num;i<devices+num;i++)
			{
				strcpy(save_name[i], save_name[i+1]);
				strcpy(save_address[i], save_address[i+1]);
			}
	
			devices--;
			return 1;
		}
		else
		{return 0;}	
	}

}
void TouchDraw()
{
	setBaggroundPixel(0,0,0);
	drawletters("Enter", 15 ,200,31,63,31);
	drawletters("Back", 90,200,31,63,31);
	drawletters("Up",185,200,31,63,31);
	drawletters("Down", 230,200,31,63,31);
	setBaggroundPixel(31,63,31);
}
void cleanDisplay()
{
	FillRectangle(0,0,320,240,31,63,31);
}
void toggleSwitch()
{
	if(locked == 2)
	{
		drawletters("LOCK AUTO", start_position_x,start_position_y+60,31,63,31); //make pixel whites
		drawletters("LOCK AUTO", start_position_x,start_position_y+60,0,0,0); //make text
	}
	else if(locked == 1)
	{
		drawletters("LOCK AUTO", start_position_x,start_position_y+60,31,63,31); //make pixel whites
		drawletters("LOCK OFF", start_position_x,start_position_y+60,0,0,0); //make text
	}
	else
	{
		drawletters("LOCK AUTO", start_position_x,start_position_y+60,31,63,31); //make pixel whites
		drawletters("LOCK ON", start_position_x,start_position_y+60,0,0,0); //make text
	}
}
void MakeLogo()
{
	int xposition = 10; 
	drawXletter(LogoBitmaps,4,LogoDescriptors[7]-LogoDescriptors[5],xposition,10,LogoDescriptors[5],LogoDescriptors[5]%8,31,0,0);
	xposition = xposition+32;
	drawXletter(LogoBitmaps,5,LogoDescriptors[5]-LogoDescriptors[3],xposition,10,LogoDescriptors[3],LogoDescriptors[3]%8,31,0,0);
	xposition = xposition+39;
	drawXletter(LogoBitmaps,3,LogoDescriptors[3]-LogoDescriptors[1],xposition,10,LogoDescriptors[1],LogoDescriptors[1]%8,31,0,0);
	xposition = xposition+18;
	drawXletter(LogoBitmaps,4,540-LogoDescriptors[7],xposition,10,LogoDescriptors[7],LogoDescriptors[7]%8,31,0,0);
	xposition = xposition+35;
	drawXletter(LogoBitmaps,5,LogoDescriptors[5]-LogoDescriptors[3],xposition,10,LogoDescriptors[3],LogoDescriptors[3]%8,31,0,0);
}
void backgroundprocess(int idle_state)
{
	if(locked == 2 && idle_state == 0)
	{
		drawletters("UPDATE", start_position_x,start_position_y+90,0,0,0);
		int lock_flag = 0;
		nearbyDevicesScan();
		for (int i=0; i<4; i++)
		{
			variable2 = getNearbyDeviceAddress(i+1);
			get_address[i] = malloc(strlen(variable2)+1); //0 terminering
			strcpy(get_address[i],variable2);
			for (int j=0; j<4; j++)
			{
				if (strcmp(save_address[j],get_address[i]) ==  0)
				{
					locked = 1;
					lock();
					lock_flag = 1;
				}
			}
		}
		if (lock_flag == 0)
		{
			locked = 0;
			lock();
		}
	locked = 2;
	drawletters("UPDATE", start_position_x,start_position_y+90,31,63,31);
	}
	
}