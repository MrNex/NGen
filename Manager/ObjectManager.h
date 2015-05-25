#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "../Data/LinkedList.h"
#include "../GObject/GObject.h"
#include "../Data/OctTree.h"
#include "../Data/HashMap.h"

typedef struct ObjectBuffer
{
	LinkedList* toDelete;
	LinkedList* gameObjects;
	OctTree* octTree;
} ObjectBuffer;

//Internal
static ObjectBuffer* objectBuffer;

///
//Allocates a new Object buffer
//
//Returns:
//	Returns a pointer to a newly allocated Object Buffer
static ObjectBuffer* ObjectManager_AllocateBuffer(void);

///
//Initializes an Object Buffer
//
//Parameters:
//	buffer: The object buffer to initialize
static void ObjectManager_InitializeBuffer(ObjectBuffer* buffer);

///
//Frees resources being used by an Object Buffer
//
//Parameters:
//	buffer: The object buffer to free
static void ObjectManager_FreeBuffer(ObjectBuffer* buffer);

//Functions

///
//Initializes the Object manager
void ObjectManager_Initialize(void);

///
//Frees all internal data managed by the Object Manager
void ObjectManager_Free(void);

///
//Gets the internal Object Buffer being managed by the Object Manager
//
//Returns:
//	Internal Object Buffer of Object Manager
ObjectBuffer ObjectManager_GetObjectBuffer(void);

///
//Updates the internal state of all contained objects.
void ObjectManager_Update(void);

///
//Updates the internal state of the OctTree
void ObjectManager_UpdateOctTree(void);

///
//Adds an object to collection of objects managed by the Object Manager
//
//Parameters:
//	obj: The object to add
void ObjectManager_AddObject(GObject* obj);

///
//Removes an object from the collection of objects managed by the Object Manager
//Upon removal the Object Manager will NOT Deallocate memory being used
//By the object.
//
//Parameters:
//	obj: The object to remove
void ObjectManager_RemoveObject(GObject* obj);

///
//Removes an object from the collection of objects being managed by the object manager.
//Upon removal the object manager will deallocate memory being used by the object.
//
//Parameters:
//	obj: the object to remove and free
void ObjectManager_DeleteObject(GObject* obj);

///
//Queues an object to be deleted at the end of the current update cycle
//
//Parameters:
//	obj: A pointer to the object to queue the deletion of
void ObjectManager_QueueDelete(GObject* obj);

#endif
