/*
  YunBoard.h - YunBoard library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef YunBoard_h
#define YunBoard_h

#include "Arduino.h"
#include <YunClient.h>
#include "Board.h"

#include "FileIO.h"

#define SD_PATH "/mnt/sda1/"
#define SD_PATH_LENGTH sizeof(SD_PATH)
#define FILENAME_LENGTH 12
#define TOTAL_PATH_LENGTH SD_PATH + FILENAME_LENGTH

#define MAX_FILES_TODEALWITH 10

class YunBoard : public Board {
  public:
    virtual void begin();
    virtual void createFile(char*);
    virtual void write(char*, byte, char*);
    virtual long fileSize(char*);
    virtual void relabelFile(char*, char*, byte, byte);
    virtual bool nextPathInDir(char*, char*, char*);
    virtual void readFile(char*, char*, unsigned long);
    virtual void getTimestamp(char*);

  protected:
    char filesInDir[MAX_FILES_TODEALWITH][TOTAL_PATH_LENGTH];
    void createFileList(char*, char*);
    bool matchesFilter(const char*, byte, const char*, byte);
    void openDir(char*);

};

#endif
