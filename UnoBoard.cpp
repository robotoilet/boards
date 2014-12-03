/*
  UnoBoard.h - UnoBoard library - implementation
  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include "Board.h"
#include "UnoBoard.h"

#include <SPI.h>
#include <SD.h>

void UnoBoard::begin() {
	chipSelect = 10;
	pinMode(SS, OUTPUT);
	// see if the card is present and can be initialized:
  	if (!SD.begin(chipSelect)) {
    	Serial.println(F("Card failed, or not present"));
    	// don't do anything more:
    	while (1) ;
  	} Serial.println(F("card initialized."));
}//end begin

File UnoBoard::openFile(char* dir) {
  return SD.open(dir);
}
File UnoBoard::openFile(char* dir, byte mode) {
  return SD.open(dir, mode);
}
