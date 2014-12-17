/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include "Board.h"
#include "UnoBoard.h"

#include <Wire.h>
#include <sdFat.h>
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

void UnoBoard::write(char* filePath, char* data, byte dataLength) {
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

void UnoBoard::createFileList(char* dirPath, const byte filepathLength, char* suffixFilter, byte labelLength) {
  Serial.println("Creating file list for directory " + String(dirPath));
  SdFile dir(dirPath, O_CREAT | O_WRITE | O_EXCL);   //init file
  SdFile f;
  filesInDir = new char*[MAX_FILES_TODEALWITH];  //create char array for list of files

  byte i = 0;
  // open next file in root.  The volume working directory, vwd, is root
  while(f.openNext(sd.vwd(), O_READ)) {
    if (i >= MAX_FILES_TODEALWITH) break;
    
    char* fName = new char[filepathLength];
    //fName[0]= {'\0'};
    f.getFilename(fName);
    if (matchesFilter(fName, filepathLength, suffixFilter, labelLength)) {
      filesInDir[i] = new char[filepathLength];
      strcpy(filesInDir[i], fName);
      i ++;  
    }
    delete[] fName;
  }
}

// string s, string-length sl, filter f, filterlength fl
bool UnoBoard::matchesFilter(const char* s, byte sl, const char* f, byte fl) {
  for (byte i=0;i<fl;i++) {
    if (s[(sl - 1 - i)] != f[fl - 1 - i]) {
      return false;
    }
  }
  return true;
}

// dirPath: path to the directory to check,
// pathBuffer: where to write the next matching path
// filepathLength: number of chars of the path to write
// suffixFilter: last `labelLength` characters of filename we want
bool UnoBoard::nextPathInDir(char* dirPath, char* pathBuffer, const byte filepathLength, char* suffixFilter, byte labelLength) {
  if (!filesInDir) {
    createFileList(dirPath, filepathLength, suffixFilter, labelLength);
  }
  for (byte i=0;i<MAX_FILES_TODEALWITH;i++) {
    if (filesInDir != NULL && filesInDir[i][0] != '\0') {
      strcpy(pathBuffer, filesInDir[i]);
      delete[] filesInDir[i];
      filesInDir[i] = NULL;
      return true;
    }
  }
  delete[] filesInDir;
  filesInDir = NULL;
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
