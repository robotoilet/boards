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
    virtual void write(char*, byte, char*);
    virtual void openDir(char*);
    virtual long fileSize(char*);
    virtual void relabelFile(char*, char*, byte, byte);


  protected:
    void openDir(char*);
    bool dirIsOpen = false;
    File dir;
};

#endif


