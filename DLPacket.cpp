/*
An Arduino library to transmit analog and digital values in a single packet of data through the serial port.
 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 06/06/2017.
 */

#include "Arduino.h"
#include "Stream.h"
#include "DLPacket.h"

boolean DLPacket::packetFull(){
  return _analogIndex + _digitalIndex == 16;
}

boolean DLPacket::addAnalogValue(int aValue){
	if( packetFull() ){
		return false;
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
	//bits 0..3 = total number of values
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
