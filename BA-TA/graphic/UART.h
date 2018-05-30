void InitUART(long choosenBaudRate, char choosenNumberOfDataBits, char choosenParity);
char ReadChar();
void SendChar(char CharToSend);
void SendString(char* StringToSend);