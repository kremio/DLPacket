/*
 Provides a minimal mock implementation of an Arduiono Stream class to be used to test the DLPacket library.

 Created by Jonathan Cremieux<kremio.software@gmail.com> for Design Research Lab<drlab.org> on 12/06/2017.
 */

#ifndef Stream_h
#define Stream_h

class Stream {
	public:
		Stream(){ _bufferPtr = _buffer;}
		size_t  write(const uint8_t *buffer, size_t  size){
			std::memcpy(_bufferPtr, buffer, size);
			_bufferPtr = _bufferPtr + size;
			return size;
		}
		size_t write(uint8_t byte){
			*(_bufferPtr) = byte;
			_bufferPtr = _bufferPtr + 1;
			return 1;
		}
		uint8_t * getBuffer(){return _buffer;}
		size_t getBufferSize(){return _bufferPtr - _buffer;}
		void reset(){ _bufferPtr = _buffer;}
	private:
		uint8_t *_bufferPtr;
		uint8_t _buffer[2+1+16];
};
#endif
