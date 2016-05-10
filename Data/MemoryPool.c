#include "MemoryPool.h"

#include <stdlib.h>

///
//Allocates a new memory pool
//
//Returns:
//	A pointer to an uninitialized memory pool
MemoryPool* MemoryPool_Allocate(void)
{
	return malloc(sizeof(MemoryPool));
}

///
//Initializes a new memory pool
//
//Parameters:
//	pool: A pointer to the memory pool to initialize
//	dataSize: The size of the data to be stored in this memory pool
void MemoryPool_Initialize(MemoryPool* pool, unsigned int dataSize)
{
	pool->pool = DynamicArray_Allocate();
	DynamicArray_Initialize(pool->pool, dataSize);

	pool->freeQueue = LinkedList_Allocate();
	LinkedList_Initialize(pool->freeQueue);
}

///
//Frees memory allocated by a memory pool
//
//Parameters:
//	pool: A pointer to the memory pool to free
void MemoryPool_Free(MemoryPool* pool)
{
	DynamicArray_Free(pool->pool);
	LinkedList_Free(pool->freeQueue);
	free(pool);
}

///
//Requests a new space in memory from the memory pool
//
//Parameters:
//	pool: A pointer to the pool to request memory from
unsigned int MemoryPool_RequestID(MemoryPool* pool)
{
	unsigned int index;
	if(pool->freeQueue->size > 0)
	{
		index = (unsigned int)pool->freeQueue->head->data;
		LinkedList_RemoveIndex(pool->freeQueue, 0);
	}
	else
	{
		if(pool->pool->size == pool->pool->capacity)
			DynamicArray_Grow(pool->pool);
		
		index = pool->pool->size;
	}
	pool->pool->size++;

	return index;
}

///
//Releases a space in memory to be reused by the memory pool it belongs to
//
//Parameters:
//	pool: A pointer to the memory pool to release memory back to
//	id: The ID of the memory unit being released
void MemoryPool_ReleaseID(MemoryPool* pool, const unsigned int id)
{
	pool->pool->size--;
	LinkedList_Append(pool->freeQueue, (void*)id);
}

///
//Requests the memory address for a given memory unit ID
//
//Parameters:
//	pool: A pointer to the pool the address resides in
//	id: The ID of the desired address
//
//Returns:
//	A pointer to the first byte of the memory unit referenced by the given ID
void* MemoryPool_RequestAddress(MemoryPool* pool, const unsigned int id)
{
	return DynamicArray_Index(pool->pool, id);
}
