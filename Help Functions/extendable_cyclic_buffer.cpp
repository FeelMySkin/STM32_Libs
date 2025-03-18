#ifndef EXTENDABLE_CYCLIC_BUFFER_CPP
#define EXTENDABLE_CYCLIC_BUFFER_CPP

#include "extendable_cyclic_buffer.h"

template <typename A>
Extendable_cyclic_Buffer<A>::Extendable_cyclic_Buffer()
{
	size = 0;
}
//

template <typename A>
void Extendable_cyclic_Buffer<A>::push(A a)
{	
	if(size == 0)
	{
		buff = new C_buffer;
		buff->next = buff;
		buff->buffer = a;
		size++;
		return;
	}
	
	C_buffer *c = buff;
	for(int i = 0;i<size-1;++i) c = c->next;

	c->next = new C_buffer;
	c=c->next;
	c->buffer = a;
	c->next = buff;
	size++;
}
//

template <typename A>
A Extendable_cyclic_Buffer<A>::get(uint16_t pos)
{
	C_buffer *c = buff;
	for(int i = 0;i<pos;++i)
	{
		c = c->next;
	}
	return c->buffer;
}
//

template <typename A>
void Extendable_cyclic_Buffer<A>::clear()
{
	C_buffer *c_buff=buff;
	for(int i  = 0;i<size;++i)
	{
		c_buff=c_buff->next;
		delete buff;
		buff = c_buff;
	}
	buff = NULL;
	size = 0;
}
//

template <typename A>
uint16_t Extendable_cyclic_Buffer<A>::length()
{
	return size;
}
//

#endif

