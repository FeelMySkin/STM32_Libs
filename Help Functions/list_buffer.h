#ifndef LIST_BUFFER_H
#define LIST_BUFFER_H

#include "defines.h"

template <class T>
class ListBuffer
{
	public:
		ListBuffer();
		void Add(T n);
		T& operator[](uint16_t idx);
		uint16_t Size() {return size;}
	
	private:
		
	uint16_t size;
	struct Buffer
	{
		T buffer;
		Buffer *next;
	}*buffer;
	//
	
	

};
//

#include "list_buffer.cpp"

#endif
