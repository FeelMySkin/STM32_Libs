#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include "defines.h"


template <typename A>
class Cyclic_Buffer
{
	public:
		Cyclic_Buffer();
		Cyclic_Buffer(uint16_t sz);
		void SetSize(uint16_t sz);
		void push(A a);
		A pull();
		A get(uint16_t pos=0);
		uint16_t* GetHeadPointer() {return head;}
		A* GetBufferPointer() {return buffer;}
		void SetBufferPointer(A* buf) {this->buffer = buf;}
		void shift(uint16_t pos);
		void clear();
		void SetOverwrite(bool);
		void SetHeadPointer(uint16_t* ptr);
		uint16_t length();
	
	private:
		bool overwrite;
		uint16_t *head;
		uint16_t tail;
		A *buffer;
		uint16_t SZ;
};
//

#include "cyclic_buffer.cpp"

#endif
