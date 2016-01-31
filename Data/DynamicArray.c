#include "DynamicArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

///
//Internal Declarations
static int defaultCapacity = 8;
static float defaultGrowth = 2.0f;


///
//Allocates a Dynamic Array
//
//Returns:
//	Pointer to a newly allocated dynamic array
DynamicArray* DynamicArray_Allocate()
{
	DynamicArray* arr = (DynamicArray*)malloc(sizeof(DynamicArray));
	arr->capacity = defaultCapacity;
	arr->growthRate = defaultGrowth;
	return arr;
}

///
//Initializes a Dynamic Array
//
//Parameters:
//	arr: Dynamic array to initialize
void DynamicArray_Initialize(DynamicArray* arr, unsigned int dataSize)
{
	arr->size = 0;
	arr->dataSize = dataSize;
	arr->data = calloc(dataSize, arr->capacity);
}

///
//Frees resources being used by a dynamic array
//
//Parameters:
//	arr: The dynamic array to free
void DynamicArray_Free(DynamicArray* arr)
{
	free(arr->data);
	free(arr);
}

///
//Copies the contents and attributes of another dynamicArray to this one
//Will resize the dynamic array if it is not big enough
//If the array is larger than the one being copied, then the memory will be copied over to the start of the new
//location but the new array will not be shrunk to match the size. Any memory which is not overwritten will
//remain unchanged.
//
//Parameters:
//	arr: A pointer to the dynamic array which is the destination of the copy
//	toCopy: A pointer to the dynamic array being copied
void DynamicArray_Copy(DynamicArray* arr, DynamicArray* toCopy)
{
	arr->growthRate = toCopy->growthRate;

	if(arr->dataSize != toCopy->dataSize)
	{
		unsigned int temp = arr->dataSize;
		arr->dataSize = toCopy->dataSize;
		if(temp < toCopy->dataSize)
		{
			DynamicArray_Grow(arr);
		}
	}

	while(toCopy->capacity > arr->capacity)
	{
		DynamicArray_Grow(arr);
	}

	memcpy(arr->data, toCopy->data, toCopy->dataSize * toCopy->capacity);
	arr->size = toCopy->size;
}

///
//Appends a dynamic array with data
//
//Parameters:
//	arr: Dynamic array to append
//	data: Data to append to array
void DynamicArray_Append(DynamicArray* arr, void* data)
{
	if (arr->size == arr->capacity) DynamicArray_Grow(arr);
	memcpy(((char*)arr->data) + (arr->dataSize * arr->size), data, arr->dataSize);
	arr->size++;
}


///
//Indexes a dynamic array
//
//Parameters:
//	arr: The dynamic array to index
//	index: The index to get
//
//Returns:
//	Pointer to data at the index of arr
void* DynamicArray_Index(DynamicArray* arr, unsigned int index)
{
	return ((char*)arr->data) + (arr->dataSize * index);
}


///
//Clears a dynamic array setting all indices & size to 0
//Does not lessen capacity
//
//Parameters:
//	arr: Pointer to dynamic array being cleared
void DynamicArray_Clear(DynamicArray* arr)
{
	memset(arr->data, 0, arr->capacity * arr->dataSize);
	arr->size = 0;
}

///
//Removes an element from the dynamic array.
//Then copies all data back one space to ensure no gaps in data exist.
//
//Parameters:
//	arr: A pointer to the dynamic array to remove an element from
//	index: The index of the element to remove
void DynamicArray_RemoveAndReposition(DynamicArray* arr, const unsigned int index)
{
	//Copy memory following the element to be deleted over the element to be deleted
	//Get a pointer to the element being deleted
	void* dstPointer = (char*)arr->data + (index * arr->dataSize);
	//Get a pointer to the element after the element being deleted
	void* srcPointer = (char*)arr->data + ((index + 1) * arr->dataSize);
	//Get the size of the memory to copy back one index
	unsigned int size = (arr->size - (index + 1)) * arr->dataSize;

	//If the size is 0 we can't overwrite the data, it is the last element in the list!
	if(size == 0)
	{
		memset(dstPointer, 0, arr->dataSize);
	}
	//Overwrite the data being removed with the data proceeding it
	else
	{
		//Copy the memory
		memmove(dstPointer, srcPointer, size);

		//Now there is a duplicate of the last entry, remove it!
		dstPointer = (char*)arr->data + ((arr->size - 1) * arr->dataSize);
		memset(dstPointer, 0, arr->dataSize);
	}

	//Decrement the size of the dynamic array
	arr->size--;
}

///
//Removes an element from the dynamic array.
//
//Parameters:
//	arr: A pointer to the dynamic array to remove an element from
//	index: The index of the element to remove
void DynamicArray_Remove(DynamicArray* arr, const unsigned int index)
{
	if(index >= arr->capacity)
	{
		printf("DynamicArray_Remove failed! Index is out of bounds of array capacity!\n");
		return;
	}
	memset((char*)arr->data + (index * arr->dataSize), 0, arr->dataSize);
	arr->size--;
}

///
//Removes a specified data from the dynamic array
//Then copies all data back one space to ensure no gaps in data exist
//
//Parameters:
//	arr: A pointer to the dynamic array to remove data from
//	data: The data to remove
void DynamicArray_RemoveDataAndReposition(DynamicArray* arr, void* data)
{
	for(unsigned int i = 0; i < arr->capacity; i++)
	{
		if(memcmp(data, (char*)arr->data + (i * arr->dataSize), arr->dataSize) == 0)
		{
			DynamicArray_RemoveAndReposition(arr, i);
			break;
		}
	}
}

///
//Removes specified data from the dynamic array
//
//Parameters:
//	arr: A pointer to the dynamic array to remove data from
//	data: The data to remove
void DynamicArray_RemoveData(DynamicArray* arr, void* data)
{
	for(unsigned int i = 0; i < arr->capacity; i++)
	{
		if(memcmp(data, (char*)arr->data + (i * arr->dataSize), arr->dataSize) == 0)
		{
			DynamicArray_Remove(arr, i);
			break;
		}
	}
}

///
//Determines if data is already contained in the array
//Searches the entire array
//
//Parameters:
//	arr: The array to search for the data
//	data: The data being searched for
//
//Returns:
//	0 if the data is not contained
//	1 if the data is contained
unsigned char DynamicArray_Contains(DynamicArray* arr, void* data)
{
	for(unsigned int i = 0; i < arr->capacity; i++)
	{
		if(memcmp(data, (char*)arr->data + (i * arr->dataSize), arr->dataSize) == 0)
		{
			return 1;
		}
	}
	return 0;
}

///
//Determines if data is already contained in the first n elements of the array
//
//Parameters:
//	arr: The array to search for the data
//	data: The data being searched for
//	n: The number of elements to search from the start of the array
//
//Returns:
//	0 if the data is not contained
//	1 if the data is contained
unsigned char DynamicArray_ContainsWithin(DynamicArray* arr, void* data, unsigned int n)
{
	for(unsigned int i = 0; i < n; i++)
	{
		if(memcmp(data, (char*)arr->data + (i * arr->dataSize), arr->dataSize) == 0)
		{
			return 1;
		}
	}
	return 0;
}



//Internal functions
///
//Increases the capacity of a dynamic array by
//It's current capacity times it's growth rate
//
//Parameters:
//	arr: the array to increase in capacity
void DynamicArray_Grow(DynamicArray* arr)
{
	//This information gets lost, we need it in case things go wrong.
	unsigned int oldSize = arr->capacity;

	//Change capacity
	arr->capacity = (unsigned int)(arr->growthRate * arr->capacity);

	//Allocate new larger memory
	void* newPtr = malloc(arr->dataSize * arr->capacity);
	memcpy(newPtr, arr->data, oldSize * arr->dataSize);
	free(arr->data);
	arr->data = newPtr;
}
