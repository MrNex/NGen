#ifndef HASHMAP_H
#define HASHMAP_H

#include "DynamicArray.h"

typedef struct HashMap
{
	//unsigned int capacity;
	//unsigned int size;
	//struct HashMap_KeyValuePair** data;
	DynamicArray* data;
	unsigned long(*Hash)(void* key, unsigned int keyLength);

} HashMap;

struct HashMap_KeyValuePair
{
	void* key;
	unsigned int keyLength;	//Size in bytes of key
	void* data;
};

///
//Internals
///

///
//Allocates memory for a Key Value Pair for the HashMap data structure
//
//Returns:
//	A pointer to a newly allocated Key Value Pair
struct HashMap_KeyValuePair* HashMap_KeyValuePair_Allocate();

///
//Initializes a key value pair
//
//Parameters:
//	pair: The key value pair being initialized
//	key: A null terminated character array containing the key to map the data to
//	data: A pointer to the data to be contained in this key value pair
//	keyLength: The size of the key in bytes
void HashMap_KeyValuePair_Initialize(struct HashMap_KeyValuePair* pair, void* key, void* data, unsigned int keyLength);

///
//Frees memory being used by a Key Value Pair
//Does not delete data!!
//
//PArameters:
//	pair: Key value pair to free
void HashMap_KeyValuePair_Free(struct HashMap_KeyValuePair* pair);

///
//Functions
///

///
//Allocates memory for a new HashMap
//
//Returns:
//	Pointer to newly allocated hashmap
HashMap* HashMap_Allocate(void);

///
//Initializes a HashMap
//
//Parameters:
//	map: Hashmap to initialize
void HashMap_Initialize(HashMap* map, unsigned int capacity);

///
//Frees a hashmap
//Does not free data!! Delete prior to this! OR have other references!!!
//
//Parameters:
//	map: The Hashmap to free
void HashMap_Free(HashMap* map);

///
//Adds data to a hashmap
//
//Parameters:
//	map: Map to add to
//	key: Key to retrieve data later
//	data: pointer to The data being added
//	keyLength: The size of the key in bytes
void HashMap_Add(HashMap* map, void* key, void* data, unsigned int keyLength);

///
//Removes an entry from the hashmap
//
//Parameters:
//	map: Map to remove entry from
//	key: Key relating to data to be removed
//	keyLength: The size of the key in bytes
void* HashMap_Remove(HashMap* map, void* key, unsigned int keyLength);

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
struct HashMap_KeyValuePair* HashMap_LookUp(HashMap* map, void* key, unsigned int keyLength);

///
//Checks if a key is contained within the hashmap
//
//Parameters:
//	map: The map to search
//	key: The key to search for
//	keyLength: The length of the key in bytes
unsigned char HashMap_Contains(HashMap* map, void* key, unsigned int keyLength);



#endif
