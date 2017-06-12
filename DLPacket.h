/*
 An Arduino library to transmit analog and digital values in a single packet of data through the serial port.

 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 06/06/2017.
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
		boolean addAnalogValue(int aValue);
		boolean addDigitalValue(boolean dValue);
		boolean packetFull();
		boolean send();
	private:
		Stream& _stream;
		byte _header[2+1] = {'d','l',0};//2 packet tag bytes, 1 header byte
		int _digitalIndex;
		byte _digital[2]; //max 16 bits of digital data
		int _analogIndex;
		byte _analog[15]; //max 15 bytes of analog values
};
#endif
