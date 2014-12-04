/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include <YunClient.h>
#include "Board.h"
#include "YunBoard.h"

#include "FileIO.h"

void YunBoard::begin() {
  Bridge.begin();
  FileSystem.begin();
}

void YunBoard::createFile(char* filePath) {
  File f = FileSystem.open(filePath, FILE_WRITE);
  f.close();
}

void YunBoard::write(char* filePath, byte filePathLength, char* data) {
  File f = FileSystem.open(filePath, FILE_WRITE);
  for (byte i=0;i<filePathLength;i++) {
    f.print(data[i]);
  }
  f.close();
}

void YunBoard::openDir(char* dirPath) {
  dir = FileSystem.open(dirPath);
  dirIsOpen = true;
}

char* Yunboard::nextPathInDir(char* dirPath) {
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

long YunBoard::fileSize(char* fPath) {
  File f = FileSystem.open(fPath);
  long size = f.size();
  f.close();
  return size;
}

void YunBoard::readFile(char* fPath, char* buffer, unsigned long checksumByteSum) {
  File f = FileSystem.open(fPath);
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

void YunBoard::relabelFile(char* fileName, char* label, byte labelSize, byte labelIndex){
  Process rename;
  rename.begin("mv");
  rename.addParameter(fileName);
  for (byte i=0; i<labelSize; i++) {
    fileName[labelIndex + i] = label[i];
  }
  rename.addParameter(fileName);
  rename.run();
  fileName[labelIndex] = '\0';
}
