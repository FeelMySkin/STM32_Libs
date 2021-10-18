#ifndef DYNAMIC_QUEUE_H
#define DYNAMIC_QUEUE_H

#include "defines.h"

template <class T>
class DynamicQueue
{
	public:
		DynamicQueue();
		void push(T);
		bool IsExists(T);
		T pop();
		T get(uint16_t);
		uint16_t GetSize();
		
	private:
		void Reallocate(uint16_t size);
		uint16_t size;
		uint16_t alloc_size;
		T *body;
	//
	
	

};
//

#include "dynamic_queue.cpp"

#endif
