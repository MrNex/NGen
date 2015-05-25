#include "Hash.h"

#include <string.h>

///
//A quick 'n dirty implementation of the sdbm public domain hash. 
//Only for string keys
//
//Parameters:
//	key: The key to hash
//
//Returns:
//	The hashvalue of the key
unsigned long Hash_SDBM(void* key, unsigned int keyLength)
{
	char* charKey = (char*) key;
	unsigned long hash = 0;
	unsigned char byteVal;

	for(unsigned int i = 0; i < keyLength; i++)
	{
		byteVal = charKey[i];
		hash = byteVal + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}
