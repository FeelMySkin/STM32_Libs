#ifndef CYCLIC_BUFFER_CPP
#define CYCLIC_BUFFER_CPP

#include "cyclic_buffer.h"

template <typename A>
Cyclic_Buffer<A>::Cyclic_Buffer()
{
	head = new uint16_t();
	SZ = 2;
	tail = 0;
	*head = 0;
	buffer = new A[SZ];
}
//
template <typename A>
Cyclic_Buffer<A>::Cyclic_Buffer(uint16_t sz)
{
	head = new uint16_t();
	SZ = sz;
	tail = 0;
	*head = 0;
	buffer = new A[SZ];
}
//

template <typename A>
void Cyclic_Buffer<A>::SetSize(uint16_t sz)
{
	SZ = sz;
	delete[] buffer;
	buffer = new A[SZ];
}
//

template <typename A>
void Cyclic_Buffer<A>::push(A a)
{
	if((*head+1)%SZ == tail && overwrite)
	{
		buffer[*head] = a;
		(*head)++;
		*head%=SZ;
		tail=(tail+1)%SZ;
	}
	else if((*head+1)%SZ != tail)
	{
		buffer[*head] = a;
		(*head)++;
		*head%=SZ;
	}

}
//

template <typename A>
A Cyclic_Buffer<A>::pull()
{
	if(tail == *head) return 0;
	A a = buffer[tail];
	tail++;
	tail%=SZ;
	return a;
}
//

template <typename A>
void Cyclic_Buffer<A>::SetOverwrite(bool setter)
{
	this->overwrite = setter;
}
//

template <typename A>
void Cyclic_Buffer<A>::shift(uint16_t s)
{
	if(tail==*head) return;
	else if(tail>*head)
	{
		if(tail+s<SZ)
		{
			tail+=s;
		}
		else if((tail+s)%SZ>*head)
		{
			tail=*head;
		}
		else
		{
			tail = (tail+s)%SZ;
		}
	}
	else if(tail<*head)
	{
		if(tail+s>*head)
		{
			tail=*head;
		}
		else tail+=s;
	}
}
//

template <typename A>
void Cyclic_Buffer<A>::clear()
{
	tail = *head;
}
//

template <typename A>
uint16_t Cyclic_Buffer<A>::length()
{
	if(tail<=*head) return *head-tail;
	else
	{
		return (SZ+*head)-tail;
	}
}
//

template <typename A>
A Cyclic_Buffer<A>::get(uint16_t pos)
{
	uint16_t buf_tail = tail;
	buf_tail = (buf_tail+pos)%SZ;
	return buffer[buf_tail];
}
//

template <typename A>
void Cyclic_Buffer<A>::SetHeadPointer(uint16_t* ptr)
{
	delete head;
	head = ptr;
}
//

#endif

