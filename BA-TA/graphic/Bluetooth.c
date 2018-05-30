/*
 * Bluetooth.c
 *
 * Created: 19-04-2018 10:24:09
 *  Author: 71395
 */ 

#include "Bluetooth.h"
#include "UART.h"
#include <stdio.h>

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

int numberOfRegisteredAddresses;
int numberOfNearbyAddresses;

char* trustedDevice1Address;
char* trustedDevice2Address;
char* trustedDevice3Address;
char* trustedDevice4Address;
//char* device5Address;
//char* device6Address;

char* trustedDevice1Name;
char* trustedDevice2Name;
char* trustedDevice3Name;
char* trustedDevice4Name;
//char* device5Name;
//char* device6Name;

char* nearbyDevice1Address;
char* nearbyDevice2Address;
char* nearbyDevice3Address;
char* nearbyDevice4Address;

void InitBluetoothModule()
{	
	numberOfRegisteredAddresses = 0;
	
	_delay_ms(1000);
	SendStringCRLF("AT+ORGL");
	
	_delay_ms(1000);
	SendStringCRLF("AT+RESET");
	
	_delay_ms(1000);
	SendStringCRLF("AT+ROLE=1"); //Taking Master role in future bonding. Enables inquiries which we need in order to scan for devices
	
	_delay_ms(1000); //Time critcal in order for the module MCU to load the SPP Bluetooth Profile
	SendStringCRLF("AT+INIT");
	
	_delay_ms(1500);
	SendStringCRLF("AT+INQM=0,4,4"); //Timeout is 4*1.28 seconds and maximum 4 devices for searching
	_delay_ms(1500);

}

//Function to send a CR LF (Carriage Return and Line Feed" on the end of the message that is sended to the Bluetooth module
//Without the CR LF added, the Bluetooth module won't register the AT commands.
void SendStringCRLF(char* message)
{
	char* CRLF = "\r\n";
	
	//char* finalMessage = message + CRLF; This ain't C# or java....
	char* finalMessage;
	finalMessage = malloc(strlen(message)+1+4);
	strcpy(finalMessage,message);
	strcat(finalMessage,CRLF);
	
	SendString(finalMessage);
	free(finalMessage); //Freeing the dynamic allocated memory

}

//Function to get the addresses of nearby devices, which the system can correlate and compare to the addresses on the trusted list. Thereby the system can lock/open the doors
void nearbyDevicesScan()
{
	int i = 0;
	char neabyRawAddresses[300];
	
	numberOfNearbyAddresses = 0;
	
	SendStringCRLF("AT+INQ"); //Scanning for devices
	
	//Recieving all returned information until the returned "OK" is registreded (which means there is no more incomming data)
	for(i = 0; i < 300; i++)
	{
		neabyRawAddresses[i] = ReadChar();
		
		if(neabyRawAddresses[i-2] == 'O' && neabyRawAddresses[i-1] == 'K' && neabyRawAddresses[i] =='\r')
		{
			//SendStringCRLF("OK\r was registered - now breaking for-loop");
			break;
		}
		
	}
	
	//SendString("Raw address was recieved: ");
	//SendStringCRLF(neabyRawAddresses);
	
	//Passing the data to manipulate into several usable addresses
	nearbyAddressDelimeter(neabyRawAddresses);
	
	//Debugging usage
	SendStringCRLF("");
	SendString("1st name: ");
	SendStringCRLF(nearbyDevice1Address);
	
	SendString("2st name: ");
	SendStringCRLF(nearbyDevice2Address);
	
	SendString("3st name: ");
	SendStringCRLF(nearbyDevice3Address);
	
	SendString("4st name: ");
	SendStringCRLF(nearbyDevice4Address);
	
	
}

//Function to get the address and names of potential trusted devices the user can add to the trusted list
void trustedDevicesScan()
{
	int i = 0;
	char trustedRawAddresses[300];
	
	numberOfRegisteredAddresses = 0;
	
	SendStringCRLF("AT+INQ");
		
	for(i = 0; i < 300; i++)
	{
		trustedRawAddresses[i] = ReadChar();
		
		if(trustedRawAddresses[i-2] == 'O' && trustedRawAddresses[i-1] == 'K' && trustedRawAddresses[i] =='\r')
		{
			//SendStringCRLF("OK\r was registered - now breaking for-loop");
			break;
		}
		
	}	
	
	//SendString("Raw address was recieved: ");
	//SendStringCRLF(trustedRawAddresses);
	
	//Splitting the address into what we need to ask for name on device
	trustedAddressDelimeter(trustedRawAddresses);
	
	/*
	SendStringCRLF("");
	SendString("1st address: ");
	SendStringCRLF(trustedDevice1Address);
	
	SendString("2st address: ");
	SendStringCRLF(trustedDevice2Address);
	
	SendString("3st address: ");
	SendStringCRLF(trustedDevice3Address);
	
	SendString("4st address: ");
	SendStringCRLF(trustedDevice4Address);
	
	
	SendString("5st address: ");
	SendStringCRLF(device5Address);
	
	SendString("6st address: ");
	SendStringCRLF(device6Address);
	*/
	
	
	//Finding the right deviceName to the respective device address
	if(strncmp(trustedDevice1Address,"N/A",256) != 0)
		trustedDevice1Name = getFoundDeviceName(trustedDevice1Address);
	else
		trustedDevice1Name = "N/A";
		
	if(strncmp(trustedDevice2Address,"N/A",256) != 0)
		trustedDevice2Name = getFoundDeviceName(trustedDevice2Address);
	else
		trustedDevice2Name = "N/A";
	
	if(strncmp(trustedDevice3Address,"N/A",256) != 0)
		trustedDevice3Name = getFoundDeviceName(trustedDevice3Address);
	else
		trustedDevice3Name = "N/A";
	
	if(strncmp(trustedDevice4Address,"N/A",256) != 0)
		trustedDevice4Name = getFoundDeviceName(trustedDevice4Address);
	else
		trustedDevice4Name = "N/A";
	/*
	if(strncmp(device5Address,"N/A",256) != 0)
		device5Name = getFoundDeviceName(device5Address);
	else
		device5Name = "N/A";
	
	if(strncmp(device6Address,"N/A",256) != 0)
		device6Name = getFoundDeviceName(device6Address);
	else
		device6Name = "N/A";
	*/
	
	
	//Debugging usage
	SendStringCRLF("");
	SendString("1st name: ");
	SendStringCRLF(trustedDevice1Name);
	
	SendString("2st name: ");
	SendStringCRLF(trustedDevice2Name);
	
	SendString("3st name: ");
	SendStringCRLF(trustedDevice3Name);
	
	SendString("4st name: ");
	SendStringCRLF(trustedDevice4Name);
	/*
	SendString("5st name: ");
	SendStringCRLF(device5Name);
	
	SendString("6st name: ");
	SendStringCRLF(device6Name);
	*/

	
}

//Function to search the whole data array for addresses and seperate them and concatenate them in the right order and comma beetween
void nearbyAddressDelimeter(char* nearbyRawAddress)
{
	int t = 0;
	
	char *nearbySaveptr1;
		
	char* nearbyPart1 = malloc(5);
	char* nearbyPart2 = malloc(3);
	char* nearbyPart1and2;
	char* nearbyFinalAddress;
	
	char** nearbyDataArray = malloc(3);
	char *nearbyAddressesToSend[4];
		
	//Check number of recieved "+INQ", which should be equal to number of addresses
	for(t = 0; t < 300; t++)
	{
		if(nearbyRawAddress[t-3] == '+' && nearbyRawAddress[t-2] == 'I' && nearbyRawAddress[t-1] == 'N' && nearbyRawAddress[t] == 'Q' )
		{
			numberOfNearbyAddresses++;
		}
	}
	
	//Splitting (delimetering) the MAC addresses into 3 parts
	char *delimeterColon = ":";
	char *delimeteComma = ",";

	char *nearbyHeader1 = strtok_r(nearbyRawAddress, delimeterColon, &nearbySaveptr1);
	
	int q = 0;
	
	//Adress example:
	//{h0A}E{h0A}+INQ:74E5:43:96BCDF,A010C,7FFF
	for(q = 0; q < numberOfNearbyAddresses; q++)
	{
		
		if(q >= 1)
		{
			nearbyHeader1 = strtok_r(NULL,delimeteComma, &nearbySaveptr1); 
			nearbyHeader1 = strtok_r(NULL,delimeterColon, &nearbySaveptr1);
		}
		
		nearbyDataArray[1] = strtok_r(NULL, delimeterColon, &nearbySaveptr1); //Indeholder første del af adressen = 74E5
		nearbyDataArray[2] = strtok_r(NULL, delimeterColon, &nearbySaveptr1); //Indeholder anden del af adressen  = 43
		nearbyDataArray[3] = strtok_r(NULL, delimeteComma, &nearbySaveptr1);  //Indeholder tredje del af adressen = 96BCDF
		
		//PART 1
		strcpy(nearbyPart1,nearbyDataArray[1]);
		strcat(nearbyPart1, ",");
		//SendString("Part 1 : ");
		//SendStringCRLF(part1);
		
		//PART 2
		strcpy(nearbyPart2,nearbyDataArray[2]);
		strcat(nearbyPart2, ",");
		//SendString("Part 2 : ");
		//SendStringCRLF(part2);
		
		//PART 3
		//ALREADY DONE
		//SendString("Part 3 : ");
		//SendStringCRLF(dataArray[3]);
		
		//PART 1 + 2
		nearbyPart1and2 = malloc(strlen(nearbyPart1)+strlen(nearbyPart2)+1); //0-termination
		strcpy(nearbyPart1and2,nearbyPart1);
		strcat(nearbyPart1and2,nearbyPart2);
		//SendString("Part 1+2 : ");
		//SendStringCRLF(part1and2);
		
		//PART 1and2 + 3
		nearbyFinalAddress = malloc(strlen(nearbyPart1and2)+1+6);
		strcpy(nearbyFinalAddress,nearbyPart1and2);
		strcat(nearbyFinalAddress,nearbyDataArray[3]);
		
		nearbyAddressesToSend[q] = nearbyFinalAddress;
		
		//SendString("Final address of this run: ");
		//SendStringCRLF(nearbyAddressesToSend[q]);
	}
	
	//free(part1);
	//free(part2);
	//free(dataArray);
	//free(part1and2); //Warning - may be used uninitialized in this function
	//free(finalAddress); //Warning - may be used uninitialized in this function
	
	//Setting the adresses and if not found: "N/A"
	switch(numberOfRegisteredAddresses)
	{
		case 0 :
		nearbyDevice1Address = "N/A";
		nearbyDevice2Address = "N/A";
		nearbyDevice3Address = "N/A";
		nearbyDevice4Address = "N/A";
		//device5Address = "N/A";
		//device6Address = "N/A";
		break;
		case 1 :
		nearbyDevice1Address = nearbyAddressesToSend[0];
		nearbyDevice2Address = "N/A";
		nearbyDevice3Address = "N/A";
		nearbyDevice4Address = "N/A";
		//device5Address = "N/A";
		//device6Address = "N/A";
		break;
		case 2 :
		nearbyDevice1Address = nearbyAddressesToSend[0];
		nearbyDevice2Address = nearbyAddressesToSend[1];
		nearbyDevice3Address = "N/A";
		nearbyDevice4Address = "N/A";
		//device5Address = "N/A";
		//device6Address = "N/A";
		break;
		case 3 :
		nearbyDevice1Address = nearbyAddressesToSend[0];
		nearbyDevice2Address = nearbyAddressesToSend[1];
		nearbyDevice3Address = nearbyAddressesToSend[2];
		nearbyDevice4Address = "N/A";
		//device5Address = "N/A";
		//device6Address = "N/A";
		break;
		case 4 :
		nearbyDevice1Address = nearbyAddressesToSend[0];
		nearbyDevice2Address = nearbyAddressesToSend[1];
		nearbyDevice3Address = nearbyAddressesToSend[2];
		nearbyDevice4Address = nearbyAddressesToSend[3];
		//device5Address = "N/A";
		//device6Address = "N/A";
		break;
		//case 5 :
		//trustedDevice1Address = addressesToSend[0];
		//trustedDevice2Address = addressesToSend[1];
		//trustedDevice3Address = addressesToSend[2];
		//trustedDevice4Address = addressesToSend[3];
		//device5Address = addressesToSend[4];
		//device6Address = "N/A";
		//break;
		//case 6 :
		//trustedDevice1Address = addressesToSend[0];
		//trustedDevice2Address = addressesToSend[1];
		//trustedDevice3Address = addressesToSend[2];
		//trustedDevice4Address = addressesToSend[3];
		//device5Address = addressesToSend[4];
		//device6Address = addressesToSend[5];
		//break;
		
	}
}

//Function to search the whole data array for the potential new trusted addresses and seperate them and concatenate them in the right order and comma beetween
void trustedAddressDelimeter(char* addresses)
{
	
	int q = 0;
		
	//Check number of recieved "+INQ", which should be equal to number of addresses
	for(q = 0; q < 300; q++)
	{
		if(addresses[q-3] == '+' && addresses[q-2] == 'I' && addresses[q-1] == 'N' && addresses[q] == 'Q' )
		{
			numberOfRegisteredAddresses++;
		}
	}
		
	////FOR DEBUGGING 
	//char intToDisplay2[16];
	//itoa(q,intToDisplay2,10);
	//SendString("Number of times the foor-loop runned: ");
	//SendStringCRLF(intToDisplay2);
		
	
	//Splitting the addresses into numberOfRegisteredAddresses's array
	char *delimeter2 = ":";
	char *delimeter3 = ",";
		
	char *saveptr1;
		
	char* part1 = malloc(5);
	char* part2 = malloc(3);
	char* part1and2;
	char* finalAddress;
		
	//Removing the header of the data
	char *header1 = strtok_r(addresses, delimeter2, &saveptr1);
		
	char** dataArray = malloc(3);
		
	char *addressesToSend[4];
		
	q = 0;
	
	//For each found address in the raw recieved data from the Bluetooth module
	for(q = 0; q < numberOfRegisteredAddresses; q++)
	{
		
		//Delimetering
		if(q >= 1)
		{
			header1 = strtok_r(NULL,delimeter3, &saveptr1);
			header1 = strtok_r(NULL,delimeter2, &saveptr1);
		}
			
		dataArray[1] = strtok_r(NULL, delimeter2, &saveptr1); //Første del
		dataArray[2] = strtok_r(NULL, delimeter2, &saveptr1); //Anden del
		dataArray[3] = strtok_r(NULL, delimeter3, &saveptr1);  //Tredje del
			
		//PART 1
		strcpy(part1,dataArray[1]);
		strcat(part1, ",");	
		//SendString("Part 1 : ");
		//SendStringCRLF(part1);
			
		//PART 2
		strcpy(part2,dataArray[2]);
		strcat(part2, ",");					
		//SendString("Part 2 : ");
		//SendStringCRLF(part2);
					
		//PART 3
		//ALREADY DONE
		//SendString("Part 3 : ");
		//SendStringCRLF(dataArray[3]);
					
		//PART 1 + 2
		part1and2 = malloc(strlen(part1)+strlen(part2)+1); //0-termination
		strcpy(part1and2,part1);
		strcat(part1and2,part2);
		//SendString("Part 1+2 : ");
		//SendStringCRLF(part1and2);
					
		//PART 1and2 + 3
		finalAddress = malloc(strlen(part1and2)+1+6);
		strcpy(finalAddress,part1and2);
		strcat(finalAddress,dataArray[3]);
					
		addressesToSend[q] = finalAddress;
		
		//SendString("Final address of this run: ");
		//SendStringCRLF(addressesToSend[q]);
	}
	
	//free(part1);
	//free(part2);
	//free(dataArray);
	//free(part1and2); //Warning - may be used uninitialized in this function
	//free(finalAddress); //Warning - may be used uninitialized in this function
	
	//Setting the adresses and if not found: "N/A"
	switch(numberOfRegisteredAddresses)
	{
		case 0 :
			trustedDevice1Address = "N/A";
			trustedDevice2Address = "N/A";
			trustedDevice3Address = "N/A";
			trustedDevice4Address = "N/A";
			//device5Address = "N/A";
			//device6Address = "N/A";
			
			break;
		case 1 :
			trustedDevice1Address = addressesToSend[0];
			trustedDevice2Address = "N/A";
			trustedDevice3Address = "N/A";
			trustedDevice4Address = "N/A";
			//device5Address = "N/A";
			//device6Address = "N/A";
			
			break;
		case 2 :
			trustedDevice1Address = addressesToSend[0];
			trustedDevice2Address = addressesToSend[1];
			trustedDevice3Address = "N/A";
			trustedDevice4Address = "N/A";
			//device5Address = "N/A";
			//device6Address = "N/A";
			
			break;
		case 3 :
			trustedDevice1Address = addressesToSend[0];
			trustedDevice2Address = addressesToSend[1];
			trustedDevice3Address = addressesToSend[2];
			trustedDevice4Address = "N/A";
			//device5Address = "N/A";
			//device6Address = "N/A";
			
			break;
		case 4 :
			trustedDevice1Address = addressesToSend[0];
			trustedDevice2Address = addressesToSend[1];
			trustedDevice3Address = addressesToSend[2];
			trustedDevice4Address = addressesToSend[3];
			//device5Address = "N/A";
			//device6Address = "N/A";
			
			break;
		//case 5 :
			//trustedDevice1Address = addressesToSend[0];
			//trustedDevice2Address = addressesToSend[1];
			//trustedDevice3Address = addressesToSend[2];
			//trustedDevice4Address = addressesToSend[3];
			//device5Address = addressesToSend[4];
			//device6Address = "N/A";
			//break;
		//case 6 :
			//trustedDevice1Address = addressesToSend[0];
			//trustedDevice2Address = addressesToSend[1];
			//trustedDevice3Address = addressesToSend[2];
			//trustedDevice4Address = addressesToSend[3];
			//device5Address = addressesToSend[4];
			//device6Address = addressesToSend[5];
			//break;
		
	}
		
}

//Function takes the right type of Bluetooth device MAC-address and
char* getFoundDeviceName(char* moduleAddress)
{
	char* ATRNAME = "AT+RNAME?";
	char* completeATmessage = malloc(strlen(ATRNAME)+1+14);;
	char* finalName;
	
	char *delimeter = ":";
	char *delimeter2 = "\r";
	char *saveptr1;
	char *header;
	char **data = malloc(1);
	
	strcpy(completeATmessage,ATRNAME);
	strcat(completeATmessage,moduleAddress);

	int i = 0;
	int timesOfCR = 0;
	char deviceName[80];
	
	_delay_ms(250); //Necessary in order not to get ERROR(0) from bluetooth module and ensure stability. Indeed a time critical aspect of the Bluetooth SPP profile
	
	//Asking the Bluetooth module for the name of this adress:
	SendStringCRLF(completeATmessage);
	//completeATmessage = "AT+RNAME?C0EE,FB,DAF25E"
	
	//Recieve the returned Bluetooth device name
	for(i = 0; i < 80; i++)
	{
		deviceName[i] = ReadChar();
		
		
		if(deviceName[i] == '\r')
		{
			timesOfCR = timesOfCR + 1;
			
			if(timesOfCR == 2)
			{
				//SendStringCRLF("CR and LF registrered - now breaking");
				break;
			}
		}
		
	}
	
	/* //DEBUGGING
	char intToDisplay[16];
	itoa(i,intToDisplay,10);
	SendString("For-loop has runned: ");
	SendString(intToDisplay);
	SendStringCRLF(" times");
	*/
	
	//SendString("This is the captured deviceName before delimetering: ");
	//SendStringCRLF(deviceName);
	
	
	//Use delimeters to split the real name from +RNAME: and whatever comes before that
	header = strtok_r(deviceName, delimeter, &saveptr1);
	
	data[1] = strtok_r(NULL, delimeter2, &saveptr1);
	
	//Use delimeters to split the rest away

	finalName = data[1];
	
	//SendString("This is the captured deviceName after delimetering: ");
	//SendStringCRLF(finalName);
	
	//free(completeATmessage);
	//free(data);
	
	return strdup(finalName);
	
}



//Used for debugging in development stage
int checkForReturnedOK()
{
	
	char returnedChar = ReadChar();

	if(returnedChar == 'O')
	{
		//setModuleName("AT+NAMEwasreturned");
		returnedChar = 'Q';
		return 1;
	}
	else
	{
		//setModuleName("AT+NAMEwasnotreturned");
		returnedChar = 'Q';
		return 0;
	}
	
	
}

//No longer used
void setModuleName(char* newBluetoothName)
{
	//_delay_ms(50);
	//SendString("");
}

//get function for other classes to get the trusted device names
char* getTrustedDeviceName(int i)
{
	switch (i)
	{
		case 1:
			return trustedDevice1Name;
		case 2:
			return trustedDevice2Name;
		case 3:
			return trustedDevice3Name;
		case 4:
			return trustedDevice4Name;
		default:
			return "ERROR NAME";
	}
}

//get function for other classes to get the trusted device address
char* getTrustedDeviceAddress(int i)
{
	switch (i)
	{
		case 1:
			return trustedDevice1Address;
		case 2:
			return trustedDevice2Address;
		case 3:
			return trustedDevice3Address;
		case 4:
			return trustedDevice4Address;
		default:
			return "ERROR NAME";
	}
}

//get function for other classes to get the nearby device address in order to compare it to the trusted devices list
char* getNearbyDeviceAddress(int i)
{
		switch (i)
		{
			case 1:
				return nearbyDevice1Address;
			case 2:
				return nearbyDevice2Address;
			case 3:
				return nearbyDevice3Address;
			case 4:
				return nearbyDevice4Address;
			default:
				return "ERROR NAME";
		}
}