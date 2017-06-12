/*
 Unit tests for the DLPacket library.
 
 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 12/06/2017.
 */
#include "lest.hpp"
#include "Arduino.h"
#include "Stream.h"
#include "DLPacket.cpp"

#include <iostream>
using namespace std;

const int MIN_PACKET_SIZE = 2+1+1+1; //d,l, 1 byte header, minimum 1 byte data, 1 byte checksum

const byte checksum(Stream& stream){
 	byte checksum = stream.getBuffer()[0];
	for(int i=1; i<stream.getBufferSize(); i++){
		checksum ^= stream.getBuffer()[i];
	}
	return checksum;
}

const int packetByteSize(Stream& stream){
	int totalDataCount = (stream.getBuffer()[2] & 0x0F) + 1;
	int analogDataCount = stream.getBuffer()[2] >> 4;
	int digitalDataCount = totalDataCount - analogDataCount;
	int digitalByteCount = digitalDataCount == 0 ? 0 : (digitalDataCount / 8) + 1;
	return( MIN_PACKET_SIZE - 1 ) + analogDataCount + digitalByteCount;
}

const lest::test specification[] =
{
  CASE( "Takes a Stream as constructor parameter." )
    {
	    Stream stream;
	    DLPacket dlpacket(stream);
        EXPECT( 0 ==  stream.getBufferSize() );
    },

  CASE( "Does not send an empty packet" ){
	Stream stream;
	DLPacket dlpacket(stream);
	EXPECT( !dlpacket.send() );
 	EXPECT( 0 ==  stream.getBufferSize() );
  },

  CASE( "Can take boolean data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	EXPECT( dlpacket.addDigitalValue(true) );
	EXPECT( dlpacket.send() );
	EXPECT( MIN_PACKET_SIZE == stream.getBufferSize() );
	EXPECT( stream.getBuffer()[3] == true );
  },

  CASE( "Can take 1 byte int data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	EXPECT( dlpacket.addAnalogValue(128) );
	EXPECT( dlpacket.send() );
	EXPECT( MIN_PACKET_SIZE == stream.getBufferSize() );
	EXPECT( stream.getBuffer()[3] == 128 );
  },


  CASE( "Can take 2 bytes int data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	EXPECT( dlpacket.addAnalogValue(256) );
	EXPECT( dlpacket.send() );
	EXPECT( MIN_PACKET_SIZE + 1 == stream.getBufferSize() );
	EXPECT( (stream.getBuffer()[3] | (stream.getBuffer()[4] << 8)) == 256 );
  },


  CASE( "Can take a mix of boolean and int data" ){
	Stream stream;
	DLPacket dlpacket(stream);
 	EXPECT( dlpacket.addDigitalValue(true) );
	EXPECT( dlpacket.addAnalogValue(128) );
	EXPECT( dlpacket.send() );
	EXPECT( MIN_PACKET_SIZE + 1 == stream.getBufferSize() );
	EXPECT( stream.getBuffer()[3] == 128 );
 	EXPECT( stream.getBuffer()[4] == true );
  },

  CASE( "Can take maximum 15 int data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	for(int i =1; i<=15; i++){
		EXPECT( dlpacket.addAnalogValue(i) );
	}
	EXPECT( dlpacket.packetFull() );
	EXPECT( !dlpacket.addAnalogValue(16) );
  },

  CASE( "Can take maximum 16 boolean data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	for(int i =1; i<=16; i++){
		EXPECT( dlpacket.addDigitalValue(i % 2 == 0) );
	}
	EXPECT( dlpacket.packetFull() );
	EXPECT( !dlpacket.addDigitalValue(false) );
  },

  CASE( "Can take maximum 16 mixed data" ){
	Stream stream;
	DLPacket dlpacket(stream);
	for(int i =1; i<=16; i++){
		if( i % 2 == 0 ){
			EXPECT( dlpacket.addDigitalValue( (i & 2) == 0) );
		}else{
			EXPECT( dlpacket.addAnalogValue(i) );
		}
	}
	EXPECT( dlpacket.packetFull() );
	EXPECT( !dlpacket.addAnalogValue(17) );
	EXPECT( !dlpacket.addDigitalValue(false) );
  },

  CASE( "Empties the packet after send" ){
	Stream stream;
	DLPacket dlpacket(stream);
	for(int i =1; i<=16; i++){
		if( i % 2 == 0 ){
			EXPECT( dlpacket.addDigitalValue( (i & 2) == 0) );
		}else{
			EXPECT( dlpacket.addAnalogValue(i) );
		}
	}
	EXPECT( dlpacket.packetFull() );
	EXPECT( dlpacket.send() );
	EXPECT( !dlpacket.packetFull() );
	for(int i =1; i<=16; i++){
		if( i % 2 == 0 ){
			EXPECT( dlpacket.addDigitalValue( (i & 2) == 0) );
		}else{
			EXPECT( dlpacket.addAnalogValue(i) );
		}
	}
	EXPECT( dlpacket.packetFull() );
  },

  CASE( "Produces correct headers and packets of matching byte length" ){
	Stream stream;
	DLPacket dlpacket(stream);
	for(int i =1; i<=16; i++){
		if( i % 2 == 0 ){
			EXPECT( dlpacket.addDigitalValue( (i & 2) == 0) );
		}else{
			EXPECT( dlpacket.addAnalogValue(i) );
		}
	}
	EXPECT( dlpacket.send() );
 	EXPECT( stream.getBuffer()[0] == 'd' );
 	EXPECT( stream.getBuffer()[1] == 'l');
	EXPECT( (stream.getBuffer()[2] & 0x0F) == 15 ); //The manifest should list 16 values (0 = 1 to 15 = 16)
	EXPECT( (stream.getBuffer()[2] >> 4) == 8 ); //of which 8 are int
	EXPECT( stream.getBufferSize() == packetByteSize(stream));

	stream.reset();
	dlpacket.addDigitalValue(true);
 	EXPECT( dlpacket.send() );
 	EXPECT( stream.getBuffer()[0] == 'd' );
 	EXPECT( stream.getBuffer()[1] == 'l');
	EXPECT( (stream.getBuffer()[2] & 0x0F) == 0 ); //The manifest should list 1 value
	EXPECT( (stream.getBuffer()[2] >> 4) == 0 ); //of which none are int
	EXPECT( stream.getBufferSize() == packetByteSize(stream));


	stream.reset();
	dlpacket.addAnalogValue(128);
 	EXPECT( dlpacket.send() );
 	EXPECT( stream.getBuffer()[0] == 'd' );
 	EXPECT( stream.getBuffer()[1] == 'l');
	EXPECT( (stream.getBuffer()[2] & 0x0F) == 0 ); //The manifest should list 1 value
	EXPECT( (stream.getBuffer()[2] >> 4) == 1 ); //of which 1 is int
	EXPECT( stream.getBufferSize() == packetByteSize(stream));


	stream.reset();
	for(int i =1; i<=16; i++){
		EXPECT( dlpacket.addDigitalValue(i % 2 == 0) );
	}
	EXPECT( dlpacket.send() );
 	EXPECT( stream.getBuffer()[0] == 'd' );
 	EXPECT( stream.getBuffer()[1] == 'l');
	EXPECT( (stream.getBuffer()[2] & 0x0F) == 15 ); //The manifest should list 16 values
	EXPECT( (stream.getBuffer()[2] >> 4) == 0 ); //of which none is int
	EXPECT( stream.getBufferSize() == packetByteSize(stream));
	
	stream.reset();
	for(int i =1;  dlpacket.addAnalogValue(i); i++){;}
	EXPECT( dlpacket.send() );
 	EXPECT( stream.getBuffer()[0] == 'd' );
 	EXPECT( stream.getBuffer()[1] == 'l');
	EXPECT( (stream.getBuffer()[2] & 0x0F) == 14 ); //The manifest should list 15 values
	EXPECT( (stream.getBuffer()[2] >> 4) == 15 ); //of which 15 are int
	EXPECT( stream.getBufferSize() == packetByteSize(stream));
  },

  CASE( "Produces correct checksums" ){
	Stream stream;
	DLPacket dlpacket(stream);
 	for(int i =1; i<=16; i++){
		if( i % 2 == 0 ){
			EXPECT( dlpacket.addDigitalValue( (i & 2) == 0) );
		}else{
			EXPECT( dlpacket.addAnalogValue(i) );
		}
	}
	EXPECT( dlpacket.send() );
	EXPECT(checksum(stream) == 0);
	
	stream.reset();
	dlpacket.addDigitalValue(true);
 	EXPECT( dlpacket.send() );
 	EXPECT(checksum(stream) == 0);


	stream.reset();
	dlpacket.addAnalogValue(128);
 	EXPECT( dlpacket.send() );
 	EXPECT(checksum(stream) == 0);


	stream.reset();
	for(int i =1; i<=16; i++){
		EXPECT( dlpacket.addDigitalValue(i % 2 == 0) );
	}
	EXPECT( dlpacket.send() );
 	EXPECT(checksum(stream) == 0);

	stream.reset();
	for(int i =1;  dlpacket.addAnalogValue(i); i++){;}
	EXPECT( dlpacket.send() );
 	EXPECT(checksum(stream) == 0);
  },



};

int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}
