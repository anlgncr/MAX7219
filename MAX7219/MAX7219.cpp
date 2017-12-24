#include "MAX7219.h"

MAX7219::MAX7219(unsigned char _matrixCount, unsigned int _bufferSize, char _cs){
  cs = _cs;
  bufferSize = _bufferSize;
  matrixCount = _matrixCount;
  
  pinMode(cs, OUTPUT);
  SPI.begin();
  
  buffer = calloc(bufferSize, sizeof(char));
  matrix_ekran = calloc(matrixCount*8, sizeof(char));

  wakeUp();
  setScanLimit(0x07);
  noDecode();
  setIntensity(0x08);
  noTestDisplay();
  clearScreen();
}

void MAX7219::update(){
	pickBytesFrom(matrix_ekran, buffer, x);
	sendBytes();
}

//matrix_ekran' daki veriler ekranın dışında mı?
bool MAX7219::isOutOfScreen(){
	if(x > bufferSize -1 || x < (-8*matrixCount-1))
		return true;
	
	return false;
}

void MAX7219::resetBuffer(){
	x = 0;
	lineIndex = 0;
	
	for(int i=0; i<bufferSize; i++)
		buffer[i] = 0x00;
}


void MAX7219::setX(int _x){
	if(_x > bufferSize || _x < (-8*matrixCount))
		return;
	
	x = _x;
}

int MAX7219::getX(){
	return x;
}

//ASCII' ye karşılık gelen karakteri 5 byte olarak çevirir
void MAX7219::AtoCharacter(unsigned char character, unsigned int nextIndex){
  if(nextIndex < bufferSize / 6){ // her karakter 6 byte yer kaplıyor
	  nextIndex *=6;
  
	  switch(character){
		case 253: character = 128; break; // ı
		case 221: character = 129; break; // İ
		case 240: character = 130; break; // ğ
		case 208: character = 131; break; // Ğ
		case 252: character = 132; break; // ü
		case 220: character = 133; break; // Ü
		case 254: character = 134; break; // ş
		case 222: character = 135; break; // Ş
		case 246: character = 136; break; // ö
		case 214: character = 137; break; // Ö
		case 231: character = 138; break; // ç
		case 199: character = 139; break; // Ç
	  }
 
	  for (int i = 0; i < 5; i++)
		buffer[nextIndex + i] = pgm_read_byte_near(&ASCII[character - 0x20][i]);
	
	buffer[nextIndex + 5] = 0; // Son byte; 1 boşluk bırakmak için
  }
}

void MAX7219::setLineIndex(unsigned int index){
	lineIndex = index;
}
unsigned int MAX7219::getLineIndex(){
	return lineIndex;
}

//Karakter dizisindeki karakteri teker teker çevirir.
void MAX7219::write(unsigned char * _text){
	while(*_text){
		AtoCharacter(*_text++, lineIndex);
		lineIndex ++;
	}
}

void MAX7219::addChar(unsigned char character){
	AtoCharacter(character, lineIndex);
	lineIndex++;
}

void MAX7219::draw(unsigned char *image, unsigned int width, unsigned int btyeIndex){
	for(unsigned int i=0; i<width; i++){
		if(btyeIndex+i < bufferSize)
			buffer[btyeIndex+i] = image[i];
		else
			break;
	}
}

void MAX7219::drawFromFlash(unsigned char *image, unsigned int width, unsigned int btyeIndex){
	for(unsigned int i=0; i<width; i++){
		if(btyeIndex+i < bufferSize)
			buffer[btyeIndex+i] = pgm_read_byte_near(image+i);
		else
			break;
	}
}

//matrixCount sayısı kadar cihaza komut gonderir.
void MAX7219::sendCommand(unsigned char cmd, unsigned char data){
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(cs, LOW);
	for(unsigned char i=0; i<matrixCount; i++){
		SPI.transfer(cmd);
		SPI.transfer(data);
	}
	digitalWrite(cs, HIGH);
	SPI.endTransaction();
}

//matrix_ekran dizisindeki verileri gonderir.
void MAX7219::sendBytes(){
	SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
	for(char reg = 0; reg<8; reg++){
		digitalWrite(cs, LOW);
		for(char matrix = 1; matrix<matrixCount+1; matrix++){
			SPI.transfer(reg+1);
			SPI.transfer(matrix_ekran[matrixCount - matrix][reg]);
		}
		digitalWrite(cs, HIGH);
	}
	SPI.endTransaction();
}

//matrix_ekran dizisindeki verileri sıfırlar.
void MAX7219::clearScreen(){
	for(unsigned char i=0; i<matrixCount; i++)
		for(char j=0; j<8; j++)
			matrix_ekran[i][j] = 0x00;
		
	sendBytes();
}

//text dizisindeki verileri x'den başlayarak matrixCount*8 kadar veri keser ve tempArray dizisinde saklar.  
void MAX7219::pickBytesFrom(unsigned char (*tempArray)[8], unsigned char *buffer, int x){
	unsigned char nextByte = 0;
	int maxLimit = bufferSize - 8*matrixCount;
	int minLimit = -matrixCount*8;
	
	if( x>-1 && x<maxLimit ){
		char i,j;
		for(i=0; i<matrixCount; i++)
			for(j=0; j<8; j++){
				tempArray[i][j] = buffer[x+nextByte];
				nextByte ++;
			}
	}
	else if( x<0 && x>minLimit ){
		char i,j;
		for(i=0; i<matrixCount; i++)
			for(j=0; j<8; j++){
				if(nextByte < -x)
					tempArray[i][j] = 0;
				else
					tempArray[i][j] = buffer[x+nextByte];
				
				nextByte ++;
			}
	}
	else if( x<bufferSize && x>maxLimit -1){
		char i,j;
		int limit = bufferSize-x;
		for(i=0; i<matrixCount; i++)
			for(j=0; j<8; j++){
				if(nextByte < limit)
					tempArray[i][j] = buffer[x+nextByte];
				else
					tempArray[i][j] = 0;
				
				nextByte ++;
			}
	}
	else{
		clearScreen();
	}
	
}


void MAX7219::decode(){
	sendCommand(0x09, 0x01);
}
void MAX7219::noDecode(){
	sendCommand(0x09, 0x00);
}
void MAX7219::setIntensity(unsigned char intensity){
	sendCommand(0x0A, intensity & 0x0F);
}
void MAX7219::setScanLimit(unsigned char limit){
	sendCommand(0x0B, limit & 0x07);
}
void MAX7219::shutDown(){
	sendCommand(0x0C, 0x00);
}
void MAX7219::wakeUp(){
	sendCommand(0x0C, 0x01);
}
void MAX7219::testDisplay(){
	sendCommand(0x0F, 0x01);
}
void MAX7219::noTestDisplay(){
	sendCommand(0x0F, 0x00);
}
















