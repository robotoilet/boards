#include "Arduino.h"
#include "FileIO.h"
#include "YunClient.h"

#define LOGDIR  "/mnt/sda1/"
#define LOGDIR_WITHOUT_SLASH  "/mnt/sda1"
#define LOGDIR_LENGTH  sizeof(LOGDIR) - 1

#define TIMESTAMP_LENGTH 10                    // number of chars of a unix ts

#define CLOSED_SUFFIX 'C'
#define LOG_SUFFIX "L"
#define LABEL_LENGTH 1

#define DOT '.'
#define DOT_LENGTH 1

#define FILEPATH_LENGTH LOGDIR_LENGTH + TIMESTAMP_LENGTH + DOT_LENGTH + LABEL_LENGTH

#define DATAPOINT_MAX 5


char fUnixTimestamp[TIMESTAMP_LENGTH + 1];
char filePath[FILEPATH_LENGTH + 1];
byte dataPointCounter = 0;

void getTimestamp(char* tsArray) {
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

void createFile(char* fPath) {
  File f = FileSystem.open(fPath, FILE_WRITE);
  f.close();
}

void resetFilepath() {
  filePath[0] - '\0';
  strcpy(filePath, LOGDIR);
}

void createNewDataFile(){
  resetFilepath();
  getTimestamp(fUnixTimestamp);
  byte i = 0;
  while (filePath[i] != '\0') {
    i++;
  }
  byte k = 0;
  for (i;i<FILEPATH_LENGTH;i++) {
    if (k == 8) { filePath[i] = DOT;
    } else if (k < 8) {
      filePath[i] = fUnixTimestamp[k];
    } else {
      filePath[i] = fUnixTimestamp[k - 1];
    }
    k++;
  }
  filePath[i] = '\0';
  strcat(filePath, LOG_SUFFIX);
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

bool checkCounter() {
  Serial.print("dp" + String(dataPointCounter) + "-");
  boolean bol;
  if (dataPointCounter < DATAPOINT_MAX) {
    dataPointCounter ++;
    bol = true;
  } else {
    Process rename;
    rename.begin("mv");
    Serial.println(filePath);
    rename.addParameter(filePath);
    filePath[FILEPATH_LENGTH - 1] = 'C';
    Serial.println(filePath);
    rename.addParameter(filePath);
    rename.run();

    // 2. create a new file with timestamped name
    createNewDataFile();
    bol = false;
    dataPointCounter = 0;
  }
  // in any case get a current timestamp to pass on to sensor(s)
  getTimestamp(fUnixTimestamp);
  return bol;
}





