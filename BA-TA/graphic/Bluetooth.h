
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