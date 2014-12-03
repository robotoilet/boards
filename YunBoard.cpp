/*
  Board.h - Board library - implementation
  Copyright (c) 2014 John Doe.  All right reserved.
*/

#include "Arduino.h"
#include <YunClient.h>
#include "Board.h"
#include "YunBoard.h"

#include "FileIO.h"

void YunBoard::begin() {
  Bridge.begin();
  FileSystem.begin();
}

File YunBoard::openFile(char* dir) {
  return FileSystem.open(dir);
}
File YunBoard::openFile(char* dir, byte mode) {
  return FileSystem.open(dir, mode);
}
