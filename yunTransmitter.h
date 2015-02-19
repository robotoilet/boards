#include "Arduino.h"

#include "YunClient.h"
#include "SPI.h"
#include <PubSubClient.h>

#include <Wire.h>

#define BYTESUM_CHARLENGTH 6
#define CHECKSUM_LENGTH TIMESTAMP_LENGTH + BYTESUM_CHARLENGTH

#define CHECK_FOR_UPDATES_DURATION 5000

#define COLLECTOR 0x1
#define STORAGE 0x2

#define CLOSE_DATAPOINT ')'

#define TIMESTAMP_LENGTH 10
#define TS_INDEX_DP 3

#define DATAPOINT_MAX 5
#define MAX_VALUE_LENGTH 10
#define MAX_DATAPOINT_LENGTH MAX_VALUE_LENGTH + TIMESTAMP_LENGTH + 3
#define DSR_BUFFER_LENGTH MAX_DATAPOINT_LENGTH * DATAPOINT_MAX

#define NO_DATA_CHAR '!'

byte server[] = {10, 10, 63, 175};  // WIFI-specifics
//byte server[] = {192, 168, 0, 7};  // WIFI-specifics

YunClient eClient;


bool requestToDelete(byte* checksum, unsigned int length) {
  char* checkSum = new char[length + 1];
  checkSum[0] = 'D'; // begin with instruction for STORAGE to delete file
  for (int i=0;i<length;i++) {
    checkSum[i-1] = checksum[i];
  }
  Serial.println("T: verifiedData: " + String(checkSum));
  Wire.beginTransmission(STORAGE);
  Wire.write(checkSum);
  Wire.endTransmission();
  delete[] checkSum;
}

// TODO: remove if unused!
void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, "verifiedData") == 0) {
    requestToDelete(payload, length);
  } else {
    Serial.println("T: CALLBACK with unknown topic!!!: " + String(topic));
  }
}

PubSubClient client(server, 1883, callback, eClient);

int freERam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void reportFreeRam() {
  Serial.println("Free Ram : " + String(freERam()));
}



unsigned long buildChecksum(byte i, char c, char* ts, unsigned long checksumByteSum) {
  if (i>=TS_INDEX_DP && i<TS_INDEX_DP+TIMESTAMP_LENGTH) {
    ts[i-TS_INDEX_DP] = c;
  } else if (i == TS_INDEX_DP+TIMESTAMP_LENGTH) {
    ts[i-TS_INDEX_DP] = '\0';
  }
  checksumByteSum += (byte)c;
  return checksumByteSum;
}

void checkForUpdates() {
  unsigned long start = millis();
  // keep listening for a while
  while (millis() - start <= CHECK_FOR_UPDATES_DURATION) {
    if (client.loop()) break;
  }
  Serial.println("T: no updates found.");
  client.disconnect();
}

void sendData(char* sendBuffer, char* checksum) {
  Serial.println("Trying to send " + String(sendBuffer));
  Serial.println(checksum);

  }

// TODO: namespace global vars properly!!
char sendBuffer[DSR_BUFFER_LENGTH + 1];
byte sendBufferIndex;
// checksum related vars
char ts[TIMESTAMP_LENGTH + 1];      // the first timestamp in the data
unsigned long checksumByteSum = 0;  // sum of all byte-values of the file
char checksum[CHECKSUM_LENGTH + 1]; // write the finished checksum here
bool dataReady = false;             //

void retrieveDataForSending(int dataSize) {
  if (dataReady) {
    Serial.println("still data to send");
    return;
  }
  while (Wire.available() > 0) {
    char c = Wire.read();
    Serial.print(c);
    sendBuffer[sendBufferIndex] = c;
    checksumByteSum = buildChecksum(sendBufferIndex, c, ts, checksumByteSum);
    if (c == NO_DATA_CHAR) {
      sendBuffer[sendBufferIndex] = '\0';
      Serial.println("T: checksumByteSum: " + String(checksumByteSum));
      sprintf(checksum, "%s:%ld", ts, checksumByteSum);
      dataReady = true;
      break;
    }
    sendBufferIndex++;
  }
}
void resetData(){
  sendBuffer[0] = '\0';
  sendBufferIndex = 0;
  ts[0] = '\0';
  checksumByteSum = 0;
  checksum[0] = '\0';
  dataReady = false;
}
bool transmitting = false;
void sendToServer(){
  Serial.println("hello");
  reportFreeRam();
  Serial.println(client.connected());
  Serial.println(client.connect("siteX", "punterX", "punterX"));
  // NOTE: the following should run (as tested in the two prev. lines but doesn't,
  // no idea why..
  //if (client.connected() || client.connect("siteX", "punterX", "punterX")) {
  //  transmitting = true;
  //  Serial.println("T: connected!");
  //  if (client.publish(checksum, sendBuffer)) {
  //    resetData();
  //    Serial.println("T: should have sent the stuff by now..");
  //  }
  //  checkForUpdates();
  //  transmitting = false;
  //} else {
  //  Serial.println("T: Didn't get a connection!");
  //}
}
