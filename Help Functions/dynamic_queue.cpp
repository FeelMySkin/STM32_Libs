#ifndef DYNAMIC_QUEUE_CPP
#define DYNAMIC_QUEUE_CPP

#include "dynamic_queue.h"

template <class T>
DynamicQueue<T>::DynamicQueue()
{
	size = 0;
	alloc_size = 0;
}
//

template <class T>
void DynamicQueue<T>::push(T part)
{
	if(size==alloc_size) Reallocate(alloc_size+2);
	body[size] = part;
	size++;
}
//

template <class T>
T DynamicQueue<T>::pop()
{
	//if (size == 0) return 0;
	T back = body[0];
	for(int i = 0;i<size-1;++i) body[i] = body[i+1];
	size--;
	return back;
}
//

template <class T>
T DynamicQueue<T>::get(uint16_t ptr)
{
	if (ptr > size) return NULL;
	T back = body[ptr];
	for(int i = ptr;i<size-1;++i) body[i] = body[i+1];
	size--;
	return back;
}
//

template <class T>
uint16_t DynamicQueue<T>::GetSize()
{
	return size;
}
//

template <class T>
void DynamicQueue<T>::Reallocate(uint16_t size)
{
	T* buf = new T[size];
	for(int i =0;i<alloc_size;++i) buf[i] = body[i];
	delete[] body;
	body = buf;
	alloc_size = size;
}
//


template <class T>
bool DynamicQueue<T>::IsExists(T part)
{
	for(int i = 0;i<size;++i)
	{
		if(part == body[i]) return true;
	}
	
	return false;
}
//

#endif
