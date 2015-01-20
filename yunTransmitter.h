#include "Arduino.h"

#include "YunClient.h"
#include "SPI.h"
#include <PubSubClient.h>


#define BYTESUM_CHARLENGTH 6
#define CHECKSUM_LENGTH TIMESTAMP_LENGTH + BYTESUM_CHARLENGTH

#define BUFFER_LENGTH 23 * 5


//byte server[] = {10, 10, 63, 88};  // WIFI-specifics
byte server[] = {192, 168, 0, 7};  // WIFI-specifics

// TODO: remove if unused!
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("CALLBACK!!! topic: " + String(topic));
  // handle message arrived
}

int freERam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

YunClient eClient;
PubSubClient client(server, 1883, callback, eClient);


void buildChecksum(char* checksum, char* buffer) {
  unsigned long checksumByteSum = 0; // sum of all byte-values of the file

  char ts[TIMESTAMP_LENGTH + 1];
  int i = 0;
  for (i;i<strlen(buffer);i++) {
    if (i>=TS_INDEX_DP && i<TS_INDEX_DP+TIMESTAMP_LENGTH) {
      ts[i-TS_INDEX_DP] = buffer[i];
    } else if (i == TS_INDEX_DP+TIMESTAMP_LENGTH) {
      ts[i-TS_INDEX_DP] = '\0';
      Serial.println("index of null: " + String(i-TS_INDEX_DP));
    }
    Serial.print(buffer[i]);
    checksumByteSum += (byte)buffer[i];
  }
  Serial.println("ts: " + String(ts));
  Serial.println("checksumByteSum: " + String(checksumByteSum));
  sprintf(checksum, "%s:%ld", ts, checksumByteSum);
}

bool requestDataForSending(char* sendBuffer, char* checksum) {
  Serial.println("free ram HERE: " + String(freERam()));
  if (!getDataForSending(sendBuffer)) {
    return false;
  }
  buildChecksum(checksum, sendBuffer);
  Serial.println("checksum: " + String(checksum));
  return true;
}

// 3. Data Transfer to server
void sendData() {
  if (client.connected() || client.connect("siteX", "punterX", "punterX")) {
    Serial.println("connected to server!");
    char sendBuffer[BUFFER_LENGTH + 1];
    char checksum[CHECKSUM_LENGTH + 1];
    if (requestDataForSending(sendBuffer ,checksum)) {
      Serial.println("trying to send it..");
      if (client.publish(checksum, sendBuffer)) {
        Serial.println("should have sent the stuff by now..");
        //relabelToSent(sendFilePath, SENT_SUFFIX);
      }
    } else {
      Serial.println("No data for sending available!");
    }
  } else {
    Serial.println("Didn't get a connection!");
  }
}
