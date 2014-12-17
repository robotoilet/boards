/*
  UnoBoard.h - UnoBoard library
  Copyright (c) 2014 John Doe.  All right reserved.
*/

// ensure this library description is only included once
#ifndef UnoBoard_h
#define UnoBoard_h

#include "Arduino.h"
#include "Board.h"
#include "UnoBoard.h"

#include <SdFat.h>

class UnoBoard : public Board {
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
 //   void openDir(char*);

};


#endif


