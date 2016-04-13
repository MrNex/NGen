#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include "../Data/DynamicArray.h"
#include "../Data/LinkedList.h"

typedef struct MemoryPool
{
	DynamicArray* pool;
	LinkedList* freeQueue;
} MemoryPool;

///
//Allocates a new memory pool
//
//Returns:
//	A pointer to an uninitialized memory pool
MemoryPool* MemoryPool_Allocate(void);

///
//Initializes a new memory pool
//
//Parameters:
//	pool: A pointer to the memory pool to initialize
//	dataSize: The size of the data to be stored in this memory pool
void MemoryPool_Initialize(MemoryPool* pool, unsigned int dataSize);

///
//Frees memory allocated by a memory pool
//
//Parameters:
//	pool: A pointer to the memory pool to free
void MemoryPool_Free(MemoryPool* pool);

///
//Requests a new space in memory from the memory pool
//
//Parameters:
//	pool: A pointer to the pool to request memory from
unsigned int MemoryPool_RequestID(MemoryPool* pool);

///
//Releases a space in memory to be reused by the memory pool it belongs to
//
//Parameters:
//	pool: A pointer to the memory pool to release memory back to
//	id: The ID of the memory unit being released
void MemoryPool_ReleaseID(MemoryPool* pool, const unsigned int id);

///
//Requests the memory address for a given memory unit ID
//
//Parameters:
//	pool: A pointer to the pool the address resides in
//	id: The ID of the desired address
//
//Returns:
//	A pointer to the first byte of the memory unit referenced by the given ID
void* MemoryPool_RequestAddress(MemoryPool* pool, const unsigned int id);

#endif
