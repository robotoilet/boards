/*
  Board.h - Board library - implementation

  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"

#include "YunClient.h"
#include "SPI.h"
#include <PubSubClient.h>

#include "Board.h"
#include "YunBoard.h"

//#define SERVER { 10, 10, 63, 221 }  // WIFI-specifics
//#define SERVER { 192, 168, 2, 103 }  // WIFI-specifics
//byte server[] = { 192, 168, 2, 105 };  // WIFI-specifics
byte server[] = { 192, 168, 240, 197 };  // WIFI-specifics

// TODO: remove if unused!
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("CALLBACK!!! topic: " + String(topic));
  // handle message arrived
}

int freERam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

YunClient eClient;
PubSubClient client(server, 1883, callback, eClient);

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
  byte sl = strlen(s);
  byte fl = strlen(f);
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
  path[0] = '\0';
  while(File f = dir.openNextFile()) {
    Serial.println("checking file  " + String(f.name()));
    if (matchesFilter(f.name(), suffixFilter)) {
      Serial.println("path before: " + String(path));
      strcat(path, f.name());
      Serial.println("path after: " + String(path));
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
  delete[] dataFilePath;
  filepathLength = LOGDIR_LENGTH + TIMESTAMP_LENGTH + DOT_LENGTH + LABEL_LENGTH;
  dataFilePath = new char[filepathLength + 1];
  strcpy(dataFilePath, LOGDIR);
}

// 3. Data Transfer to server
void YunBoard::sendData() {
  char* sendFilePath = new char[filepathLength + 1];
  strcat(sendFilePath, LOGDIR_WITHOUT_SLASH);
  Serial.println("free ram: " + String(freERam()));
  Serial.println("Preparing to send data");
  if (nextPathInDir(sendFilePath, CLOSED_SUFFIX)) {
    Serial.println("free ram HERE: " + String(freERam()));
    Serial.println("..for file " + String(sendFilePath));
    unsigned int bufferLength = fileSize(sendFilePath);
    char sendBuffer[bufferLength + 1];

    char checksum[CHECKSUM_LENGTH + 1] = {'\0'};
    char startEnd[10];
    // checksumBytes: sum of all byte-values of the file
    unsigned long checksumBytes = readFile(sendFilePath, sendBuffer);

    Serial.println("free ram HERE AGAIN: " + String(freERam()));
    buildChecksum(checksum, sendBuffer, bufferLength, checksumBytes);

    Serial.println("trying to send it..");
    if (client.connected() || client.connect("siteX", "punterX", "punterX")) {
      Serial.println("connected to server!");
      if (client.publish(checksum, sendBuffer)) {
        Serial.println("should have sent the stuff by now..");
        relabelFile(sendFilePath, SENT_SUFFIX);
      }
    } else {
      Serial.println("Didn't get a connection!");
    }
  }
  delete[] sendFilePath;
}
