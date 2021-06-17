#ifndef CYCLIC_BUFFER_CPP
#define CYCLIC_BUFFER_CPP

#include "cyclic_buffer.h"

template <typename A,uint16_t SZ>
Cyclic_Buffer<A,SZ>::Cyclic_Buffer()
{
	tail = 0;
	head = 0;
}
//

template <typename A,uint16_t SZ>
void Cyclic_Buffer<A,SZ>::push(A a)
{
	if((head+1)%SZ == tail && overwrite)
	{
		buffer[head] = a;
		head++;
		head%=SZ;
		tail=(tail+1)%SZ;
	}
	else if((head+1)%SZ != tail)
	{
		buffer[head] = a;
		head++;
		head%=SZ;
	}

}
//

template <typename A,uint16_t SZ>
A Cyclic_Buffer<A,SZ>::pull()
{
	if(tail == head) return 0;
	A a = buffer[tail];
	tail++;
	tail%=SZ;
	return a;
}
//

template <typename A,uint16_t SZ>
void Cyclic_Buffer<A,SZ>::SetOverwrite(bool setter)
{
	this->overwrite = setter;
}
//

template <typename A,uint16_t SZ>
void Cyclic_Buffer<A,SZ>::shift(uint16_t s)
{
	if(tail==head) return;
	else if(tail>head)
	{
		if(tail+s<SZ)
		{
			tail+=s;
		}
		else if((tail+s)%SZ>head)
		{
			tail=head;
		}
		else
		{
			tail = (tail+s)%SZ;
		}
	}
	else if(tail<head)
	{
		if(tail+s>head)
		{
			tail=head;
		}
		else tail+=s;
	}
}
//

template <typename A,uint16_t SZ>
void Cyclic_Buffer<A,SZ>::clear()
{
	tail = head;
}
//

template <typename A,uint16_t SZ>
uint16_t Cyclic_Buffer<A,SZ>::length()
{
	if(tail<=head) return head-tail;
	else
	{
		return (SZ+head)-tail;
	}
}
//

template <typename A,uint16_t SZ>
A Cyclic_Buffer<A,SZ>::get(uint16_t pos)
{
	uint16_t buf_tail = tail;
	buf_tail = (buf_tail+pos)%SZ;
	return buffer[buf_tail];
}
//

#endif

