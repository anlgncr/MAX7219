#ifndef MAX7219_h
#define MAX7219_h
#include "arduino.h"
#include <SPI.h>
#include "ASCII.h"

class MAX7219
{
  public:
	MAX7219(unsigned char, unsigned int, char);
	void clearScreen();
	void decode();
	void noDecode();
	void setIntensity(unsigned char);
	void setScanLimit(unsigned char);
	void shutDown();
	void wakeUp();
	void testDisplay();
	void noTestDisplay();
	void update();
	void setX(int);
	int getX();
	void write(unsigned char*);
	void addChar(unsigned char);
	bool isOutOfScreen();
	void resetBuffer();
	void setLineIndex(unsigned int);
	unsigned int getLineIndex();
	void draw(unsigned char*, unsigned int, unsigned int);
	void drawFromFlash(unsigned char*, unsigned int, unsigned int);
	
  private: 
	void pickBytesFrom(unsigned char (*)[8], unsigned char *, int);
	void sendCommand(unsigned char, unsigned char);
	void sendBytes();
	void AtoCharacter(unsigned char, unsigned int);
	
	int x = 0;
	int lineIndex = 0;
	unsigned char matrixCount = 3;
	int bufferSize = 24;
	unsigned char cs = 8;
	unsigned char (*matrix_ekran)[8];
	unsigned char *buffer;
};

#endif