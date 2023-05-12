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
	return s->holder;
}
//

template <class T>
void ListBuffer<T>::Add(T n)
{
	Buffer * bf = new Buffer();
	bf->holder = n;
	bf->next = buffer;
	buffer = bf;
	size++;
	
}
//

template <class T>
T ListBuffer<T>::Delete(uint16_t n)
{
	if(size ==0 || n >= size) return;
	
	Buffer* s = buffer;
	Buffer *deleter;
	for(int i = 0;i<n-1;++i)s = s->next;
	
	if(n == 0) deleter = s;
	else deleter = s->next;
	
	if(deleter->next != NULL) s->next = deleter->next;
	if(n == 0) buffer = buffer->next;
	T returner = deleter->holder;
	delete deleter;
	size--;
	return returner;
	
}
//

#endif
