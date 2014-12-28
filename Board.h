/*
  Board.h - Board library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef Board_h
#define Board_h

#include "Arduino.h"

#define TIMESTAMP_LENGTH 10                    // number of chars of a unix ts

#define CLOSED_SUFFIX "C"
#define SENT_SUFFIX "S"
#define LOG_SUFFIX "L"

#define LABEL_LENGTH sizeof(CLOSED_SUFFIX) - 1

#define DOT '.'
#define DOT_LENGTH 1

#define BYTESUM_CHARLENGTH 6
#define CHECKSUM_LENGTH TIMESTAMP_LENGTH + BYTESUM_CHARLENGTH


class Board
{
  public:
    Board(byte cM) : counterMax(cM),dataPointCounter(0) {}

    char unixTimestamp[TIMESTAMP_LENGTH + 1];
    virtual void begin() = 0;
    virtual void createFile(char*);
    virtual void write(char*, byte) = 0;
    virtual long fileSize(char*) = 0;
    virtual void renameFile(char*, char*) = 0;
    virtual bool nextPathInDir(char*, char*) = 0;
    virtual unsigned long readFile(char*, char*) = 0;
    virtual void getTimestamp(char*) = 0;
    virtual void sendData() = 0;

    bool checkCounter() {
      Serial.print("dp" + String(dataPointCounter) + "-");
      boolean bol;
      if (dataPointCounter < counterMax) {
        dataPointCounter ++;
        bol = true;
      } else {
        relabelFile(dataFilePath, CLOSED_SUFFIX);
        // 2. create a new file with timestamped name
        createNewDataFile();
        bol = false;
        dataPointCounter = 0;
      }
      // in any case get a current timestamp to pass on to sensor(s)
      getTimestamp(unixTimestamp);
      return bol;
    }

    void relabelFile(char* oldName, char* label) {
      char newName[filepathLength + 1];
      newName[0] = '\0';
      strcat(newName, oldName);
      for (byte i=0; i<LABEL_LENGTH; i++) {
        newName[filepathLength - 1 - i] = label[LABEL_LENGTH - 1 - i];
      }
      renameFile(oldName, newName);
    }

    void createNewDataFile(){
      resetDataFilePath();
      getTimestamp(unixTimestamp);
      byte i = 0;
      while (dataFilePath[i] != '\0') {
        i++;
      }
      byte k = 0;
      for (i;i<filepathLength;i++) {
        if (k == 8) { dataFilePath[i] = DOT;
        } else if (k < 8) {
          dataFilePath[i] = unixTimestamp[k];
        } else {
          dataFilePath[i] = unixTimestamp[k - 1];
        }
        k++;
      }
      dataFilePath[i] = '\0';
      strcat(dataFilePath, LOG_SUFFIX);
      createFile(dataFilePath);
    }

  protected:
    virtual void resetDataFilePath() = 0;
    char* dataFilePath;
    byte filepathLength;
    byte dataPointCounter;
    byte counterMax;

    void buildChecksum(char* checksum, char* buffer, unsigned int bufferLength, unsigned long bytes) {
      sprintf(checksum, "%ld", bytes);
      for (byte i=strlen(checksum); i<CHECKSUM_LENGTH; i++) {
        if (i < BYTESUM_CHARLENGTH) {
          checksum[i] = ':';
        } else {
          checksum[i] = buffer[bufferLength - (CHECKSUM_LENGTH - i)];
        }
      }
      checksum[CHECKSUM_LENGTH] = '\0';
    }
};

#endif

