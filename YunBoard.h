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


class YunBoard : public Board {
  public:
    virtual void begin();
    virtual void createFile(char*);
    virtual void write(char*, char*, byte);
    virtual long fileSize(char*);
    virtual void renameFile(char*, char*);
    virtual bool nextPathInDir(char*, char*, const byte, char*, byte);
    virtual unsigned long readFile(char*, char*);
    virtual void getTimestamp(char*);

  protected:
    char** filesInDir = NULL;
    void createFileList(char*, byte, char*, byte);
    bool matchesFilter(const char*, byte, const char*, byte);
    void openDir(char*);

};

#endif
