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
    virtual File openFile(char*) = 0;
    virtual File openFile(char*, byte) = 0;

//  protected:
};

#endif

