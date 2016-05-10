#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "../Data/LinkedList.h"
#include "../GObject/GObject.h"
#include "../Data/OctTree.h"
#include "../Data/HashMap.h"
#include "../Data/MemoryPool.h"

typedef struct ObjectBuffer
{
	LinkedList* toDelete;
	LinkedList* gameObjects;
	OctTree* octTree;

	MemoryPool* objectPool;
} ObjectBuffer;

//Internal
extern ObjectBuffer* objectBuffer;

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
//Requests an Object Memory Unit ID with which to create a new GObject
//
//returns:
//	Unsigned int containing the ID of the GObject in the ObjectManager's objectPool.
unsigned int ObjectManager_RequestObject(void);

//TODO: This should have nothing to do with Objects, only with colliders.
///
//Adds an object with a collider to the Object Manager's internal oct tree
//
//Parameters:
//	objID: The memory unit ID of the object in the object manager's objectPool to register into the OctTree system
void ObjectManager_RegisterObject(unsigned int objID);

///
//Queues an object memory unit to be Released an Object Memory Unit
//Removal will occur at the end of each Object Manager update cycle
//
//Parameters:
//	objID: The id of the object memory unit to be queued for release
void ObjectManager_QueueReleaseObject(unsigned int objID);

///
//Retrieves an object given it's objectPool memory unit ID
//
//Parameters:
//	objID: The memory unit ID of the desired object in the objectPool
//
//Returns:
//	A pointer to the desired object
GObject* ObjectManager_LookupObject(unsigned int objID);

//TODO: Remove 4

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
