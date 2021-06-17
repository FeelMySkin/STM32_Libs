#include "counter_list.h"

CounterList::CounterList()
{
	size = 0;
}
//

void CounterList::AddCounter(void* ptr,uint8_t size)
{
	ListStruct* last = str;
	
	
	
	for(int i = 1;i<this->size;++i) last = last->next;
	if(this->size == 0)
	{	
		last = new ListStruct();
		str = last;
	}
	else
	{
		last->next = new ListStruct();
		last = last->next;
	}
	last->ptr = ptr;
	last->size = size;
	this->size++;
}
//

void CounterList::IncreaseAll()
{
	ListStruct *buf = str;
	for(int i = 0;i<size;++i)
	{
		switch(buf->size)
		{
			case 1:
				(*((uint8_t*)buf->ptr))++;
			break;
			
			case 2: 
				(*((uint16_t*)buf->ptr))++;
			break;
			
			case 4:
				(*((uint32_t*)buf->ptr))++;
			break;
			
			case 8:
				(*((uint64_t*)buf->ptr))++;
			break;
			
			default:
				break;
		}
		buf = buf->next;
	}
}
//

void CounterList::DecreaseAll()
{
	ListStruct *buf = str;
	for(int i = 0;i<size;++i)
	{
		switch(buf->size)
		{
			case 1:
				(*((uint8_t*)buf->ptr))--;
			break;
			
			case 2: 
				(*((uint16_t*)buf->ptr))--;
			break;
			
			case 4:
				(*((uint32_t*)buf->ptr))--;
			break;
			
			case 8:
				(*((uint64_t*)buf->ptr))--;
			break;
			
			default:
				break;
		}
		buf = buf->next;
	}
}
//

void CounterList::RemoveCounter(void* ptr)
{
	if(size == 0) return;
	uint32_t pp = 0;
	ListStruct *buf = str;
	if(buf->ptr != ptr)
	{
		for(pp = 0;pp<size-1;++pp)
		{
			if(buf->next->ptr == ptr) break;
			buf = buf->next;
		}
		if(pp == size-1) return;
		if(pp != size-2)
		{
			ListStruct *buf2 = buf->next;
			buf->next = buf->next->next;
			delete buf2;
		}
		else delete buf->next;
	}
	else
	{
		str = str->next;
		delete buf;
	}
	size--;
	
}
//