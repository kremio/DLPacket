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

#include "Arduino.h"
#include "Stream.h"
#include "DLPacket.h"

boolean DLPacket::packetFull(){
  return _analogIndex == 15 || (_analogIndex + _digitalIndex) == 16;
}

boolean DLPacket::addAnalogValue(int aValue){
	if( packetFull() ){
		return false;
	}
	if( aValue > 0xFF ){ //need to split the value in 2 individual bytes
		//First check that there is enough space in the packet to accommodate 2 additional bytes
		if(_analogIndex == 14 ){
			return false;
		}
		//right most byte first,left most byte last
		return addAnalogValue( aValue & 0x00FF ) && addAnalogValue( (aValue & 0xFF00) >> 8 );
	}
	_analog[_analogIndex++] = aValue;
	return true;
}

boolean DLPacket::addDigitalValue(boolean aValue){
	if( packetFull() ){
		return false;
	}
	int bytePos = _digitalIndex / 8;
	int bitPos = _digitalIndex % 8;
	if(bitPos == 0){
		_digital[bytePos] = 0;
	}
	_digital[bytePos] |= aValue << bitPos;
	_digitalIndex++;
	return true;
}

boolean DLPacket::send(){
	if(_analogIndex + _digitalIndex == 0){ //nothing to send!
		return false;
	}

	byte checksum = 0;
	//Set header:
	//bits 0..3 = total number of values (0 -> 1 value, 15 -> 16 values)
	//bits 4..7 = how many analog values
	_header[2] = (_analogIndex << 4) | (_analogIndex + _digitalIndex - 1);
	checksum ^= _header[0] ^ _header[1] ^ _header[2];
	_stream.write(_header, 3); //send the header

	int a=0;
	if(_analogIndex > 0){
		for(; a < _analogIndex; a++){
			checksum ^=  _analog[a];
		}
		_stream.write(_analog, _analogIndex);
	}

	if( _digitalIndex > 0 ){
		int digitalByteCount = (_digitalIndex / 8) + 1;
		for(a=0; a < digitalByteCount; a++){
			checksum ^=  _digital[a];
		}
		_stream.write(_digital, digitalByteCount);
	}

	_stream.write(checksum);

	//reset
	_digitalIndex = 0;
	_analogIndex = 0;
	return true;
}
