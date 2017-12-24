#include "MAX7219.h"

#define MAX_COUNT 3
#define BUFFER_SIZE 240
#define CS 8

MAX7219 ekran(MAX_COUNT, BUFFER_SIZE, CS);

void setup() {
  Serial.begin(9600);
  ekran.setIntensity(0x0f);
  ekran.setX(0);
  ekran.setLineIndex(0);
  ekran.write("MAX");
  ekran.update();
}

void loop() {

}
 









