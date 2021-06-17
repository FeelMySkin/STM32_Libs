#ifndef COUNTER_LIST_H
#define COUNTER_LIST_H

#include "stdint.h"
#include "stdlib.h"

struct ListStruct
{
	ListStruct* next;
	uint8_t size;
	void* ptr;
};

class CounterList
{
	public:
		CounterList();
		void AddCounter(void* ptr,uint8_t size);
		void RemoveCounter(void* ptr);
		void IncreaseAll();
		void DecreaseAll();
	private:
		ListStruct* str;
		uint8_t size;
};

//#include "defines.h"


#endif
