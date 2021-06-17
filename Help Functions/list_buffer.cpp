#ifndef LIST_BUFFER_CPP
#define LIST_BUFFER_CPP

#include "list_buffer.h"


template <class T>
ListBuffer<T>::ListBuffer()
{
	size = 0;
}
//


template <class T>
T& ListBuffer<T>::operator[](uint16_t idx)
{
	Buffer* s = buffer;
	for(int i = 0;i<idx;++i)s = s->next;
	return s->buffer;
}
//

template <class T>
void ListBuffer<T>::Add(T n)
{
	if(size ==0)
	{
		buffer = new Buffer();
		buffer->buffer = n;
		size++;
		return;
	}
	
	Buffer* s = buffer;
	while(s->next != 0) s = s->next;
	s->next = new Buffer();
	s = s->next;
	s->buffer = n;
	size++;
	
}
//


#endif
