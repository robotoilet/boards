/*
  UnoBoard.h - UnoBoard library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef UnoBoard_h
#define UnoBoard_h

#include "Arduino.h"
#include "Board.h"

#include <SPI.h>
#include <SD.h>


class YunBoard : public Board {
  public:
    virtual void begin();
    virtual File openFile(char*);
    virtual File openFile(char*, byte);

  protected:
    const byte chipSelect;
//    virtual void getData(char*);
};

#endif


