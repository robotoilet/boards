/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include <YunClient.h>
#include "Board.h"
#include "YunBoard.h"

#include "FileIO.h"

#define MAX_FILES_TODEALWITH 10
#define SD_PATH "/mnt/sda1/"
#define SD_LIB_FILESIZE_RESTRICTION 12
#define FILEPATH_SIZE sizeof(SD_PATH) + SD_LIB_FILESIZE_RESTRICTION
#define SUFFIX_SIZE 3

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

void YunBoard::write(char* filePath, char* data, byte dataLength) {
  File f = FileSystem.open(filePath, FILE_APPEND);
  for (byte i=0;i<dataLength;i++) {
    if (data[i] == '\0') {
      break;
    }
    f.print(data[i]);
  }
  f.close();
}

void YunBoard::createFileList(char* dirPath, char* suffixFilter) {
  File dir = FileSystem.open(dirPath);
  Serial.println("Creating file list for directory " + String(dirPath));
  long i = 0;
  while(File f = dir.openNextFile()) {
    if (matchesFilter(f.name(), FILEPATH_SIZE, suffixFilter, SUFFIX_SIZE)) {
      filesInDir[i][0] = '\0';
      strcpy(filesInDir[i], f.name());
      i ++;  
    }
  }
  // null the rest to avoid false remnants
  for (i;i< MAX_FILES_TODEALWITH;i++) {
    filesInDir[i][0] = '\0';
  }
}

// string s, string-length sl, filter f, filterlength fl
bool YunBoard::matchesFilter(const char* s, byte sl, const char* f, byte fl) {
  for (byte i=0;i<fl;i++) {
    if (s[(sl - 1 - i)] != f[fl - 1 - i]) {
      return false;
    }
  }
  return true;
}

// dirPath: path to the directory to check,
// suffixFilter: last SUFFIX_SIZE characters of filename we want
bool YunBoard::nextPathInDir(char* dirPath, char* pathBuffer, char* suffixFilter) {
  if (filesInDir[0][0] == '\0') {
    createFileList(dirPath, suffixFilter);
  }
  Serial.println("files in dir:");
  for (byte i=0;i<10;i++) {
    Serial.println(filesInDir[i]);
  }
  Serial.println("end of filelist");
  for (byte i=0;i<MAX_FILES_TODEALWITH;i++) {
    if (filesInDir[i][0] != '\0') {
      strcpy(filesInDir[i], pathBuffer);
      filesInDir[i][0] = '\0';
      Serial.println(pathBuffer);
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

void YunBoard::readFile(char* fPath, char* buffer, unsigned long checksumByteSum) {
  File f = FileSystem.open(fPath);
  //Serial.println("starting to read " + String(f.name()));
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

void YunBoard::renameFile(char* oldName, char* newName) {
  Process rename;
  rename.begin("mv");
  rename.addParameter(oldName);
  rename.addParameter(newName);
  rename.run();
}
