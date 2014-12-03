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
    virtual File openFile(char*);
    virtual File openFile(char*, byte);

//  protected:
//    virtual void getData(char*);
};

#endif


