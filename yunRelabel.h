#include "YunClient.h"

void renameFile(char* oldName, char* newName) {
  Process rename;
  rename.begin("mv");
  rename.addParameter(oldName);
  rename.addParameter(newName);
  rename.run();
}

void relabelFile(char* oldName, char* label) {
  byte nlength = strlen(oldName);
  byte llength = strlen(label);

  char newName[nlength];
  newName[0] = '\0';
  strcat(newName, oldName);
  for (byte i=0; i<llength; i++) {
    newName[nlength - 1 - i] = label[llength - 1 - i];
  }
  renameFile(oldName, newName);
  Serial.println("renamed " + String(oldName) + " to " + String(newName));
}
