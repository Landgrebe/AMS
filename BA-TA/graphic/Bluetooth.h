/*
 * Bluetooth.h
 *
 * Created: 19-04-2018 10:24:19
 *  Author: 71395
 */ 

void InitBluetoothModule();
void SendStringCRLF(char* message);

void setModuleName();

int checkForReturnedOK();

void nearbyDevicesScan();
void trustedDevicesScan();

void nearbyAddressDelimeter(char* nearbyRawAddress);
void trustedAddressDelimeter(char* trustedRawAddress);

char* getFoundDeviceName(char* moduleAddress);

char* getTrustedDeviceName(int i);
char* getTrustedDeviceAddress(int i);
char* getNearbyDeviceAddress(int i);