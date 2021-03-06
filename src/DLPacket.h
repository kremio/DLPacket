/*
 An Arduino library to transmit analog and digital values agglomerated in a single packet of data through a given Stream.
 In the context if this library 'analog' values refer to unsigned integers between 0 and 65535 and 'digital' values are
 boolean as defined in Arduino.h.

 Features:
 The packet ends with a parity check sum that can be used to ensure the integrity of the data
 and detect the end of the packet.
 The packet must contain at least 1 value before it can be sent.
 A maximum of 15 analog values and a maximum of 16 digital values are allowed.
 Analog and digital values can be combined into a maximum of 16 values in total.
 Analog values greater than 255, which are encoded on 2 bytes, are split into two
 1 byte numbers and account for two analog values.

 The packet layout:
 byte position	| Content
 in the packet	|
 -------------------------------
 0				| The ASCII char 'd' (or 100 in decimal or 64 in hexadecimal)
 1				| The ASCII char 'l' (or 108 in decimal or 6C in hexadecimal)
 2				| The packet manifest: bits 0..3 = total number of values (0 -> 1 value, 15 -> 16 values)
 				| bits 4..7 = how many analog values
 3 to n			| The payload of the packet. The analog values are sent first followed by 0 to 2 bytes of digital values
 n + 1			| The checksum: XOR sum of the byte 0 to n


 Usage:
 #include <DLPacket.h>
 DLPacket dataPacket(Serial); //Use the Serial library to transmit the packets

 void setup(){
  Serial.begin(57600);
 }

 void loop(){
 	dataPacket.addAnalogValue(128); //1 byte
 	dataPacket.addAnalogValue(true); //1 bit
 	dataPacket.addAnalogValue(1024); //2 bytes
	dataPacket.addAnalogValue(false); //1 bit
	dataPacket.send(); //send the packet
 }

 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 06/06/2017.
 BSD license, all text above must be included in any redistribution
 */

#ifndef DLPacket_h
#define DLPacket_h

#include "Arduino.h"
#include "Stream.h"

class DLPacket{
	public:
		DLPacket( Stream& stream ) : _stream(stream){
			_digitalIndex = 0;
			_analogIndex = 0;
		};
		boolean addAnalogValue(int aValue); //0 < aValue < 65535
		boolean addDigitalValue(boolean dValue); //dValue = {true | false}
		boolean packetFull(); //true is no more data can be added
		boolean send(); //transmit the packet through the Stream, returns false if the packet is empty
	private:
		Stream& _stream;
		byte _header[2+1] = {'d','l',0};//2 packet tag bytes, 1 header byte
		int _digitalIndex;
		byte _digital[2]; //max 16 bits of digital data
		int _analogIndex;
		byte _analog[15]; //max 15 bytes of analog values
};
#endif
