/*
 * TFTdriver.h
 *
 * Created: 20-04-2018 9:41:31
 *  Author: Soren
 */ 

void DisplayInit();
void DisplayOff();
void DisplayOn();
void SleepOut();
void MemoryAccessControl(unsigned char parameter);
void InterfacePixelFormat(unsigned char parameter);
void WritePixel(unsigned char Red, unsigned char Blue, unsigned char Green);
void SetColumnAddress(unsigned int Start, unsigned int End);
void SetPageAddress(unsigned int Start, unsigned int End);
void MemoryWrite();
void drawXletter(const uint8_t bitmap[],int length,int count,int startx,int starty, int letter, int modulus,unsigned char Red, unsigned char Green, unsigned char Blue);
void FillRectangle(unsigned int StartX, unsigned int StartY, unsigned int Width, unsigned int Height, unsigned char Red, unsigned char Green, unsigned char Blue);
void drawletters(char str[],int startx, int starty,unsigned char Red, unsigned char Green, unsigned char Blue);
void drawNumber(long int number,int startx, int starty,unsigned char Red, unsigned char Green, unsigned char Blue);
void setBaggroundPixel(int red, int green, int blue);
//void drawbitmap(const uint8_t bitmap[],int size,int x, int y);