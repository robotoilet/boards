/*
  Board.h - Board library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef Board_h
#define Board_h

#include "Arduino.h"

class Board
{
  public:
    virtual void begin() = 0;
    virtual void createFile(char*);
    virtual void write(char*, byte, char*) = 0;
    virtual long fileSize(char*) = 0;
    virtual void relabelFile(char*, char*, byte, byte) = 0;
    virtual bool nextPathInDir(char*, char*, char*) = 0;
    virtual void readFile(char*, char*, unsigned long) = 0;
    virtual void getTimestamp(char*) = 0;
};

#endif

