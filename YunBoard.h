/*
  YunBoard.h - YunBoard library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef YunBoard_h
#define YunBoard_h

#include "Arduino.h"
#include "Board.h"

#include "FileIO.h"

#define LOGDIR  "/mnt/sda1/"
#define LOGDIR_WITHOUT_SLASH  "/mnt/sda1"
#define LOGDIR_LENGTH  sizeof(LOGDIR) - 1


class YunBoard : public Board {
  public:
    YunBoard(byte cM) : Board(cM){};
    virtual void begin();
    virtual void createFile(char*);
    virtual void write(char*, byte);
    virtual long fileSize(char*);
    virtual void renameFile(char*, char*);
    virtual bool nextPathInDir(char*, char*);
    virtual unsigned long readFile(char*, char*);
    virtual void getTimestamp(char*);
    virtual void sendData();

  protected:
    virtual void resetFilepath();
    char** filesInDir = NULL;
    bool matchesFilter(const char*, const char*);
};

#endif
