/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include "Board.h"
#include "UnoBoard.h"

#include <Wire.h>
#include <SdFat.h>
#include "RTClib.h"
RTC_DS1307 rtc;
#define CHIP_SELECT 10
SdFat sd;


//init UnoBoard with SPI speed, SPI_HALF_SPEED = when using breadboards, SPI_FULL_SPEED for better performance
void UnoBoard::begin() {
  sd.begin(CHIP_SELECT, SPI_HALF_SPEED);
  rtc.begin();

}

void UnoBoard::getTimestamp(char* tsArray) {
  tsArray[0] = '\0';
  DateTime now = rtc.now();
  uint32_t ut = now.unixtime();  
  Serial.println(String(ut));
  sprintf(tsArray,"%ld",ut);
}

void UnoBoard::createFile(char* filePath) {
  SdFile f(filePath, O_CREAT | O_WRITE | O_EXCL);
  f.close();
}

void UnoBoard::write(char* data, byte dataLength) {
  SdFile f;
  if(!f.open(filePath, O_RDWR | O_CREAT | O_AT_END)) Serial.println(F("Error writing file"));
  for (byte i=0;i<dataLength;i++) {
    if (data[i] == '\0') {
      break;
    }
    f.print(data[i]);
    Serial.print(data[i]);
  }
  f.close();
}

/// string s, filter f
bool UnoBoard::matchesFilter(const char* s, const char* f) {
  byte sl = sizeof(s);
  byte fl = sizeof(f);
  for (byte i=0;i<fl;i++) {
    if (s[(sl - 1 - i)] != f[fl - 1 - i]) {
      return false;
    }
  }
  return true;
}

// path: where to write the next matching path
// suffixFilter: last `labelLength` characters of filename we want
bool UnoBoard::nextPathInDir(char* path, char* suffixFilter) {
  SdFile f;
  while(f.openNext(sd.vwd(), O_READ)) {
    char* fName = new char[sizeof(path)];
    f.getFilename(fName);
    if (matchesFilter(fName, suffixFilter)) {
      strcat(path, fName);
      return true;
    }
  }
  return false;
}

long UnoBoard::fileSize(char* fPath) {
  SdFile f;
  f.open(fPath);
  long size = f.fileSize();
  f.close();
  return size;
}

unsigned long UnoBoard::readFile(char* fPath, char* buffer) {
  SdFile f;
  f.open(fPath);
  unsigned long checksumByteSum = 0;
  unsigned int i = 0;
  int b = f.read();
  while (b != -1) {
    buffer[i] = (char)b;
    checksumByteSum += b;
    i++;
    b = f.read();
  }
  buffer[i] = '\0';
  f.close();
  return checksumByteSum;
}

void UnoBoard::renameFile(char* oldName, char* newName) {
  SdFile oldFile(oldName, O_CREAT | O_WRITE );   //init file
  if (!oldFile.rename(sd.vwd(),newName)) Serial.println(F("error renaming"));
}

void UnoBoard::resetFilepath() {
  delete[] filePath;
  byte fpLength = TIMESTAMP_LENGTH + DOT_LENGTH + LABEL_LENGTH;
  filePath = new char[fpLength + 1];
}

void UnoBoard::sendData() {
  // TODO: talk to other board, ask to send dataPoints
}
