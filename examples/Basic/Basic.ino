/*
Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 12/06/2017.
See: https://github.com/kremio/DLPacket

DLPacket is a convenient interface to build and send through an
Arduino Stream up to 16 individual numerical values as part of a
well defined general purpose data packet.

Its usage requires 3 steps:
*/

#include <DLPacket.h>
DLPacket dataPacket(Serial); // 1) Specify which Stream library to use to transmit the packets

void setup() {
  Serial.begin(57600);
}

void loop() {
  // 2) Add data to the packet
  dataPacket.addAnalogValue(128); //1 byte
  dataPacket.addAnalogValue(true); //1 bit
  dataPacket.addAnalogValue(1024); //2 bytes
  dataPacket.addAnalogValue(false); //1 bit
  
  // 3) Send the packet
  dataPacket.send(); //send the packet
  
  delay(500);
}
