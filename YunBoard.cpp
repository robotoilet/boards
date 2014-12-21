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

void YunBoard::getTimestamp(char* tsArray) {
  Process time;
  time.begin("date");
  time.addParameter("+%s");
  time.run();

  int i = 0;
  while(time.available()>0) {
    char c = time.read();
    if (c == '\n') {
      tsArray[i] = '\0';
      break;
    }
    tsArray[i] = c;
    i ++;
  }
}

void YunBoard::createFile(char* filePath) {
  File f = FileSystem.open(filePath, FILE_WRITE);
  f.close();
}

void YunBoard::write(char* data, byte dataLength) {
  File f = FileSystem.open(dataFilePath, FILE_APPEND);
  for (byte i=0;i<dataLength;i++) {
    if (data[i] == '\0') {
      break;
    }
    f.print(data[i]);
  }
  f.close();
}

// string s, filter f
bool YunBoard::matchesFilter(const char* s, const char* f) {
  byte sl = sizeof(s);
  byte fl = sizeof(f);
  for (byte i=0;i<fl;i++) {
    if (s[(sl - 1 - i)] != f[fl - 1 - i]) {
      return false;
    }
  }
  return true;
}

// path: where to write the next matching path (gets passed prepopulated with the directory)
// suffixFilter: last `labelLength` characters of filename we want
bool YunBoard::nextPathInDir(char* path, char* suffixFilter) {
  File dir = FileSystem.open(path);
  while(File f = dir.openNextFile()) {
    if (matchesFilter(f.name(), suffixFilter)) {
      strcat(path, f.name());
      return true;
    }
  }
  return false;
}

long YunBoard::fileSize(char* fPath) {
  File f = FileSystem.open(fPath);
  long size = f.size();
  f.close();
  return size;
}

unsigned long YunBoard::readFile(char* fPath, char* buffer) {
  File f = FileSystem.open(fPath);
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

void YunBoard::renameFile(char* oldName, char* newName) {
  Process rename;
  rename.begin("mv");
  rename.addParameter(oldName);
  rename.addParameter(newName);
  rename.run();
}

void YunBoard::resetDataFilePath() {
  filepathLength = LOGDIR_LENGTH + TIMESTAMP_LENGTH + DOT_LENGTH + LABEL_LENGTH;
  Serial.println("filePathLength = " + String(LOGDIR_LENGTH) + " + " + String(TIMESTAMP_LENGTH) + " + " + String(DOT_LENGTH) + " + " + String(LABEL_LENGTH));
  dataFilePath = new char[filepathLength + 1];
  strcpy(dataFilePath, LOGDIR);
}
