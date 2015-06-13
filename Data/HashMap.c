#include "HashMap.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Hash.h"
#include "../GObject/GObject.h"

///
//Internal Declarations

///
//Increases the internal storage of the hashmap, and re-hashes all existing key value pairs
//
//Parameters:
//	map: THe hashmap to increase the internal storage of
static void HashMap_Grow(HashMap* map);

///
//Takes an existing key value pair and re-hashes and re-adds it to the hashmap
//To be used when a hashmap is growing in size
//
//Parameters:
//	map: THe hashmap the existing pair is being added to
//	pair: THe pair to add
static void HashMap_AddPair(HashMap* map, struct HashMap_KeyValuePair* pair);

///
//Implementations

///
//Allocates memory for a new HashMap
//
//Returns:
//	Pointer to newly allocated hashmap
HashMap* HashMap_Allocate(void)
{
	HashMap* map = (HashMap*)malloc(sizeof(HashMap));
	return map;
}

///
//Initializes a HashMap
//
//Parameters:
//	map: Hashmap to initialize
void HashMap_Initialize(HashMap* map)
{
	map->data = DynamicArray_Allocate();
	DynamicArray_Initialize(map->data, sizeof(struct HashMap_KeyValuePair*));
	map->Hash = Hash_SDBM;
}

///
//Frees a hashmap
//Does not free data!! Delete prior to this! OR have other references!!!
//
//Parameters:
//	map: The Hashmap to free
void HashMap_Free(HashMap* map)
{
	//for (unsigned int i = 0; i < map->size; i++)
	for(unsigned int i = 0; i < map->data->capacity; i++)
	{
		struct HashMap_KeyValuePair* pair = NULL;
		if((pair = *(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, i)) != NULL)
		{
			HashMap_KeyValuePair_Free(pair);
			DynamicArray_Remove(map->data, i);
		}

	}

	//free(map->data);
	DynamicArray_Free(map->data);
	free(map);
}

///
//Adds data to a hashmap
//
//Parameters:
//	map: Map to add to
//	key: Key to retrieve data later
//	data: pointer to The data being added
//	keyLength: The size of the key in bytes
void HashMap_Add(HashMap* map, void* key, void* data, unsigned int keyLength)
{
	struct HashMap_KeyValuePair* pair = HashMap_KeyValuePair_Allocate();
	HashMap_KeyValuePair_Initialize(pair, key, data, keyLength);

	unsigned int index = map->Hash(key, keyLength) % map->data->capacity;
	while(*(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, index) != NULL)
	{
		index = (index + 1) % map->data->capacity;
	}

	*(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, index) = pair;
	map->data->size++;

	//If the array needs to grow
	if(map->data->size == map->data->capacity)
	{
		HashMap_Grow(map);
	}
}

///
//Removes an entry from the hashmap
//Does not delete data!
//
//Parameters:
//	map: Map to remove entry from
//	key: Key relating to data to be removed
//	keyLength: The size of the key in bytes
void* HashMap_Remove(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned short found = 0;

	unsigned int index = (map->Hash(key, keyLength) % map->data->capacity);
	struct HashMap_KeyValuePair* pairToRemove = 0;
	//for (unsigned int i = 0; i < map->capacity; i++)
	for(unsigned int i = 0; i < map->data->capacity; i++)
	{
		pairToRemove = *(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, (index + i) % map->data->capacity);
		
		if(pairToRemove == NULL)
		{
			//printf("Pair NULL..\n");
		}
		else if(keyLength == pairToRemove->keyLength)
		{
			if (memcmp(key, pairToRemove->key, pairToRemove->keyLength) == 0)
			{
				index = (index + i) % map->data->capacity;
				found = 1;
				break;
			}
		}
	}
	void* data = NULL;
	if (found == 1)
	{
		data = pairToRemove->data;
		HashMap_KeyValuePair_Free(pairToRemove);
		//DynamicArray_Remove(map->data, index);
		memset((char*)map->data->data + index * map->data->dataSize, 0, map->data->dataSize);
		map->data->size--;
	}
	return data;
}

///
//Looks up a key and returns the related data
//
//Parameters:
//	map: The HashMap to lookup data in
//	key: The key related to the data
//	keyLength: The size of the key in bytes
//
//Returns:
//	Pointer to data
struct HashMap_KeyValuePair* HashMap_LookUp(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned int index = (map->Hash(key, keyLength) % map->data->capacity);
	struct HashMap_KeyValuePair* pair = 0;
	for (unsigned int i = 0; i < map->data->capacity; i++)
	{
		pair = *(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, (index + i) % map->data->capacity);
		if (pair != NULL)
			if(keyLength == pair->keyLength)
				if (memcmp(key, pair->key, pair->keyLength) == 0) 	return pair;

	}

	return NULL;

}

///
//Checks if a key is contained within the hashmap
//
//Parameters:
//	map: The map to search
//	key: The key to search for
//	keyLength: The length of the key in bytes
unsigned char HashMap_Contains(HashMap* map, void* key, unsigned int keyLength)
{
	unsigned int index = (map->Hash(key, keyLength) % map->data->capacity);
	struct HashMap_KeyValuePair* pair = NULL;
	for(unsigned int i = 0; i < map->data->capacity; i++)
	{
		pair = *(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, (index + i) % map->data->capacity);


		if(pair == NULL)
		{
			//This cannot be assumed because what if there is a collision, then the object which initially caused the collision is removed
			//And we search for the object which was subject to the collision.
			//break;
		}
		else
		{
			if(keyLength == pair->keyLength)
			{
				if(memcmp(key, pair->key, pair->keyLength) == 0) return 1;
			}
		}
	}
	return 0;
}



//Internals

///
//Allocates memory for a Key Value Pair for the HashMap data structure
//
//Returns:
//	A pointer to a newly allocated Key Value Pair
struct HashMap_KeyValuePair* HashMap_KeyValuePair_Allocate()
{
	struct HashMap_KeyValuePair* pair = (struct HashMap_KeyValuePair*)malloc(sizeof(struct HashMap_KeyValuePair));
	return pair;
}

///
//Initializes a key value pair
//
//Parameters:
//	pair: The key value pair being initialized
//	key: A null terminated character array containing the key to map the data to
//	data: A pointer to the data to be contained in this key value pair
//	keyLength: The size of the key in bytes
void HashMap_KeyValuePair_Initialize(struct HashMap_KeyValuePair* pair, void* key, void* data, unsigned int keyLength)
{
	pair->key = (char*)malloc(sizeof(char) * keyLength);
	memcpy(pair->key, key, keyLength);
	pair->keyLength = keyLength;
	pair->data = data;
}

///
//Frees memory being used by a Key Value Pair
//Does not delete data!!
//
//PArameters:
//	pair: Key value pair to free
void HashMap_KeyValuePair_Free(struct HashMap_KeyValuePair* pair)
{
	free(pair->key);
	free(pair);
}

///
//Increases the internal storage of the hashmap, and re-hashes all existing key value pairs
//
//Parameters:
//	map: THe hashmap to increase the internal storage of
static void HashMap_Grow(HashMap* map)
{
	//Grow it
	DynamicArray_Grow(map->data);

	//Copy data
	struct HashMap_KeyValuePair** copy = (struct HashMap_KeyValuePair**)malloc(sizeof(struct HashMap_KeyValuePair*) * map->data->size);
	memcpy(copy, map->data->data, sizeof(struct HashMap_KeyValuePair*) * map->data->size);

	//Save size
	unsigned int size = map->data->size;

	//Clear dynamic array
	DynamicArray_Clear(map->data);

	//Re-Add all entries
	for(unsigned int i = 0; i < size; i++)
	{
		HashMap_AddPair(map, copy[i]);
	}
}

///
//Takes an existing key value pair and re-hashes and re-adds it to the hashmap
//To be used when a hashmap is growing in size
//
//Parameters:
//	map: THe hashmap the existing pair is being added to
//	pair: THe pair to add
static void HashMap_AddPair(HashMap* map, struct HashMap_KeyValuePair* pair)
{
	unsigned int hash = map->Hash(pair->key, pair->keyLength);
	unsigned int index = hash % map->data->capacity;

	while(*(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, index) != NULL)
	{
		index = (index + 1) % map->data->capacity;
	}

	*(struct HashMap_KeyValuePair**)DynamicArray_Index(map->data, index) = pair;
	map->data->size++;
}
