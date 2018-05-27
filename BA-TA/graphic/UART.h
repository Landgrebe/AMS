void InitUART(unsigned long BaudRate, unsigned char DataBit, char Parity);
unsigned char CharReady();
char ReadChar();
void SendChar(char Tegn);
void SendString(char* Streng);
void SendInteger(int Tal);