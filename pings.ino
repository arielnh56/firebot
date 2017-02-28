
#include <NewPing.h>

#include "settings.h"

uint8_t currentPing = 0;          // Keeps track of which sensor is active.
unsigned long int lastPing;

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(frontPing_p, frontPing_p, MAX_FRONT_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(leftPing_p, leftPing_p, MAX_SIDE_DISTANCE),
  NewPing(rightPing_p, rightPing_p, MAX_SIDE_DISTANCE)
};


void checkPings() {
  if (millis() > lastPing + PING_TIMER) {
    range[currentPing] = sonar[currentPing].ping_cm();
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x60 + currentPing * 5); // Home
    Serial2.print("     ");
    Serial2.write(0x10); // ctrl-P
    Serial2.write(0x60 + currentPing * 5); // Home
    Serial2.print(range[currentPing]);
    
    lastPing = millis();
    currentPing++;
    currentPing %= SONAR_NUM;
  }
}



