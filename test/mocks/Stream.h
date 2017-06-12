/*
 Provides a minimal mock implementation of an Arduiono Stream class to be used to test the DLPacket library.

 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 12/06/2017.
 */

#ifndef Stream_h
#define Stream_h

class Stream {
	public:
		Stream(){}
		int write(const uint8_t *buffer, int size){return size;}
		int write(uint8_t byte){return 1;}
};
#endif
