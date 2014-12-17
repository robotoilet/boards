/*
  UnoBoard.h - UnoBoard library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef UnoBoard_h
#define UnoBoard_h

#include "Arduino.h"
#include "Board.h"

#include <SdFat.h>

class UnoBoard : public Board {
  public:
    virtual void begin();
    virtual void createFile(char*);
    virtual void write(char*, byte, char*);
    virtual void openDir(char*);
    virtual long fileSize(char*);
    virtual void renameFile(char*, char*);


  protected:
    void openDir(char*);
    bool dirIsOpen = false;
    File dir;
};

#endif


