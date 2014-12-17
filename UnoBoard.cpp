/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include "Board.h"
#include "UnoBoard.h"

#include <sdFat.h>

#define CHIP_SELECT SS
#define SD_PIN 10

void UnoBoard::begin() {
  sdFat sd;
  Serial.print("Initializing sd card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SD_PIN, OUTPUT);
  // see if the card is present and can be initialized:
  // Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    sd.initErrorHalt();
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}


void UnoBoard::createFile(char* filePath) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  SdFile f = sd.open(filePath, FILE_WRITE);
  f.close();
}

void UnoBoard::write(char* filePath, byte filePathLength, char* data) {
  SdFile f = sd.open(filePath, FILE_WRITE);
  for (byte i=0;i<filePathLength;i++) {
    f.print(data[i]);
  }
  f.close();
}

void UnoBoard::openDir(char* dirPath) {
  dir = sd.open(dirPath);
  dirIsOpen = true;
}

char* UnoBoard::nextPathInDir(char* dirPath) {
  if (dirIsOpen) {
    openDir(dirPath);
  }
  File f = dir.openNextFile();
  if (f) {
    const char* fName = f.name();
    f.close();
    return fName;
  } else {
    dirIsOpen = false;
    return NULL;
  }
}

long UnoBoard::fileSize(char* fPath) {
  SdFile f = sd.open(fPath);
  long size = f.size();
  f.close();
  return size;
}

void UnoBoard::readFile(char* fPath, char* buffer, unsigned long checksumByteSum) {
  SdFile f = sd.open(fPath);
  Serial.println("starting to read " + String(f.name()));
  checksumByteSum = 0;
  unsigned int i = 0;
  int b = f.read();
  while (b != -1) {
    buffer[i] = (char)b;
    checksumByteSum += b;
    i++;
    b = f.read();
  }
  f.close();
}

void UnoBoard::renameFile(char* oldFileName, char* newFileName){

}