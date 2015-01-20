#include "Arduino.h"
#include "FileIO.h"
#include "YunClient.h"

#include "yunRelabel.h"

#define LOGDIR  "/mnt/sda1/"
#define LOGDIR_WITHOUT_SLASH  "/mnt/sda1"
#define LOGDIR_LENGTH  sizeof(LOGDIR) - 1

#define TIMESTAMP_LENGTH 10                    // number of chars of a unix ts

#define CLOSED_SUFFIX "C"
#define LOG_SUFFIX "L"
#define SENT_SUFFIX "S"
#define LABEL_LENGTH 1

#define DOT '.'
#define DOT_LENGTH 1

#define FILEPATH_LENGTH LOGDIR_LENGTH + TIMESTAMP_LENGTH + DOT_LENGTH + LABEL_LENGTH

#define DATAPOINT_MAX 5

#define TS_INDEX_DP 3


char filePath[FILEPATH_LENGTH + 1];
byte dataPointCounter = 0;

void createFile(char* fPath) {
  File f = FileSystem.open(fPath, FILE_WRITE);
  f.close();
}

void resetFilepath() {
  filePath[0] = '\0';
  strcpy(filePath, LOGDIR);
}

void createNewDataFile(char* dataPoint){
  resetFilepath();
  byte i = 0;
  while (filePath[i] != '\0') {
    i++;
  }
  byte k = 0;
  for (i;i<FILEPATH_LENGTH-1;i++) {
    if (k == 8) { filePath[i] = DOT;
    } else if (k < 8) {
      filePath[i] = dataPoint[TS_INDEX_DP + k];
    } else {
      filePath[i] = dataPoint[TS_INDEX_DP + k - 1];
    }
    k++;
  }
  filePath[i] = '\0';
  strcat(filePath, LOG_SUFFIX);
  Serial.println("creating " + String(filePath));
  createFile(filePath);
}

void writeDataPoint(char* data) {
  File f = FileSystem.open(filePath, FILE_APPEND);
  for (byte i=0;i<strlen(data);i++) {
    if (data[i] == '\0') {
      break;
    }
    f.print(data[i]);
  }
  f.close();
}

bool checkCounter(char* dataPoint) {
  Serial.print("dp" + String(dataPointCounter) + "-");
  boolean bol;
  if (dataPointCounter < DATAPOINT_MAX) {
    dataPointCounter ++;
    bol = true;
  } else {
    // 1. rename current file
    relabelFile(filePath, CLOSED_SUFFIX);
    // 2. create a new file with timestamped name
    createNewDataFile(dataPoint);
    bol = false;
    dataPointCounter = 0;
  }
  return bol;
}

// string s, filter f
bool matchesFilter(const char* s, const char* f) {
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
bool nextPathInDir(char* path, char* suffixFilter) {
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

long fileSize(char* fPath) {
  File f = FileSystem.open(fPath);
  long size = f.size();
  f.close();
  return size;
}

unsigned long readFile(char* fPath, char* buffer) {
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


unsigned long getDataForSending(char* sendBuffer) {
  char* sendFilePath = new char[FILEPATH_LENGTH + 1];
  strcat(sendFilePath, LOGDIR_WITHOUT_SLASH);
  Serial.println("Preparing to send data");
  unsigned long checksumBytes = 0;
  if (nextPathInDir(sendFilePath, CLOSED_SUFFIX)) {
    Serial.println("..for file " + String(sendFilePath));

    char startEnd[10];
    // checksumBytes: sum of all byte-values of the file
    checksumBytes = readFile(sendFilePath, sendBuffer);
  }
  delete[] sendFilePath;
  return checksumBytes;
}
