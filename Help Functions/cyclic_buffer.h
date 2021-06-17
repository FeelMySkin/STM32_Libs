#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include "defines.h"


template <typename A,uint16_t SZ>
class Cyclic_Buffer
{
	public:
		Cyclic_Buffer();
		void push(A a);
		A pull();
		A get(uint16_t pos=0);
		void shift(uint16_t pos);
		void clear();
		void SetOverwrite(bool);
		uint16_t length();
	
	private:
		bool overwrite;
		uint16_t head;
		uint16_t tail;
		A buffer[SZ];
};
//

#include "cyclic_buffer.cpp"

#endif
