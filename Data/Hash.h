#ifndef HASH_H
#define HASH_H

///
//A quick 'n dirty implementation of the sdbm public domain hash. 
//
//Parameters:
//	key: The key to hash
//
//Returns:
//	The hashvalue of the key
unsigned long Hash_SDBM(void* key, unsigned int keyLength);

#endif
