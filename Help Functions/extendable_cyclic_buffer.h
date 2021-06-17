#ifndef EXTENDABLE_CYCLIC_BUFFER_H
#define EXTENDABLE_CYCLIC_BUFFER_H

#include "defines.h"

template <typename A>
class Extendable_cyclic_Buffer
{
	public:
		Extendable_cyclic_Buffer();
		void push(A a);
		A get(uint16_t pos=0);
		void clear();
		uint16_t length();
	
	private:
		uint16_t size;
		struct C_buffer
		{
			A buffer;
			C_buffer *next;
		}*buff;
};
//

#include "extendable_cyclic_buffer.cpp"

#endif
