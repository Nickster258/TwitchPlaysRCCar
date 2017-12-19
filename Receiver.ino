// Nicholas Stonecipher
// December 12, 2017
// RECEIVER

#include <SPI.h>
#include "Motor.h"          // Modified this class so I can invert the motors on my keyboard
#include "RF24.h"           // without moving wires.  - - - - - - - - - - - - - - - - - Yes
Motor motorA(2,4,3, false);
Motor motorB(5,7,6, false);
RF24 radio(9,8);
const int PACKETSIZE = 6;
byte packet[PACKETSIZE];
int radioChannel = 131;
byte address[6] = {"[REDACTED]"};

void setup() {
  pinMode(10, OUTPUT);
  Serial.begin(115200);
  radio.begin();
  radio.setChannel(radioChannel);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1,address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int *intPacket = (int *)packet;
    radio.read(packet, 6);
    int rawX = map(intPacket[0],0,1023,-255,255);
    int rawY = map(intPacket[1],0,1023,-255,255);
    digitalWrite(10, intPacket[2]);
    int left = constrain(rawX+rawY, -255, 255);
    int right = constrain(rawX-rawY, -255, 255);
    sendToMotors(right,left);
  }
  delay(10);
}

void sendToMotors(int x, int y) {
  motorA.run(((x > 150) || (x < -150)) * x);
  motorB.run(((y > 150) || (y < -150)) * y);
}
