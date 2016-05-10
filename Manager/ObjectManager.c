#include "ObjectManager.h"
#include <stdio.h>

#include "CollisionManager.h"

///
//Internals
ObjectBuffer* objectBuffer;

///
//Static Declaratons

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

///
//Releases an object and it's internal resources from the object managers objectpool
//
//Parameters:
//	objID: The ID of the object to release
static void ObjectManager_ReleaseObject(unsigned int objID);


///
//Definitions
///

///
//Initializes the Object manager
void ObjectManager_Initialize(void)
{
	objectBuffer = ObjectManager_AllocateBuffer();
	ObjectManager_InitializeBuffer(objectBuffer);
}

///
//Frees all internal data managed by the Object Manager
void ObjectManager_Free(void)
{
	ObjectManager_FreeBuffer(objectBuffer);
}

///
//Gets the internal Object Buffer being managed by the Object Manager
//
//Returns:
//	Internal Object Buffer of Object Manager
ObjectBuffer ObjectManager_GetObjectBuffer(void)
{
	return *objectBuffer;
}

///
//Updates the internal state of all contained objects.
void ObjectManager_Update(void)
{
	//TODO: remove old ways
	/*
	struct LinkedList_Node* current = objectBuffer->gameObjects->head;
	struct LinkedList_Node* next = NULL;
	while (current != NULL)
	{
		next = current->next;
		GObject* gameObj = (GObject*)(current->data);

		GObject_Update(gameObj);

		
		//Clear the game objects list of collisions which occurred with itself last frame
		if(gameObj->collider != NULL)
		{
			if(gameObj->collider->currentCollisions->size > 0)
			{
				LinkedList_Clear(gameObj->collider->currentCollisions);
			}
		}
		

		current = next;
	}

	//Delete the to delete queue
	current = objectBuffer->toDelete->head;
	next = NULL;
	while(current != NULL)
	{
		next = current->next;
		GObject* gameObj = (GObject*)(current->data);
		ObjectManager_DeleteObject(gameObj);
		current = next;
	}

	LinkedList_Clear(objectBuffer->toDelete);
	*/

//	struct LinkedList_Node* current = objectBuffer->gameObjects->head;
//	struct LinkedList_Node* next = NULL;
//	while (current != NULL)
//	{
	for(unsigned int i = 0; i < objectBuffer->objectPool->pool->capacity; i++)
	{
		//next = current->next;
		GObject* gameObj = MemoryPool_RequestAddress(objectBuffer->objectPool, i);
	
		if(gameObj->frameOfReference != NULL)
		{
			GObject_Update(gameObj);

		
			//Clear the game objects list of collisions which occurred with itself last frame
			if(gameObj->collider != NULL)
			{
				if(gameObj->collider->currentCollisions->size > 0)
				{
					LinkedList_Clear(gameObj->collider->currentCollisions);
				}
			}
		}
		//current = next;
	}

	//Delete the to delete queue
	struct LinkedList_Node* current = objectBuffer->toDelete->head;
	struct LinkedList_Node* next = NULL;
	while(current != NULL)
	{
		next = current->next;
		unsigned int objID = (unsigned int)(current->data);
		//ObjectManager_DeleteObject(gameObj);
		ObjectManager_ReleaseObject(objID);
		current = next;
	}

	LinkedList_Clear(objectBuffer->toDelete);
}

///
//Updates the internal state of the OctTree
void ObjectManager_UpdateOctTree(void)
{
	//OctTree_Update(objectBuffer->octTree, objectBuffer->gameObjects);
	OCtTree_UpdateWithMemoryPool(objectBuffer->octTree, objectBuffer->objectPool);
}

///
//Requests an Object Memory Unit ID with which to create a new GObject
//
//returns:
//	Unsigned int containing the ID of the GObject in the ObjectManager's objectPool.
unsigned int ObjectManager_RequestObject(void)
{
	unsigned int objID = MemoryPool_RequestID(objectBuffer->objectPool);
	return objID;
}

//TODO: This should have nothing to do with Objects, only with colliders.
///
//Adds an object with a collider to the Object Manager's internal oct tree
//
//Parameters:
//	objID: The memory unit ID of the object in the object manager's objectPool to register into the OctTree system
void ObjectManager_RegisterObject(unsigned int objID)
{
	GObject* obj = ObjectManager_LookupObject(objID);
	if(obj->collider != NULL)
	{
		OctTree_AddAndLog(objectBuffer->octTree, obj);
	}
}

///
//Queues an object memory unit to be Released an Object Memory Unit
//Removal will occur at the end of each Object Manager update cycle
//
//Parameters:
//	objID: The id of the object memory unit to be queued for release
void ObjectManager_QueueReleaseObject(unsigned int objID)
{
	LinkedList_Append(objectBuffer->toDelete, (void*)objID);
}

///
//Releases an object and it's internal resources from the object managers objectpool
//
//Parameters:
//	objID: The ID of the object to release
static void ObjectManager_ReleaseObject(unsigned int objID)
{
	GObject* GO = ObjectManager_LookupObject(objID);

	if(GO->collider != NULL)
	{
		OctTree_RemoveAndUnLog(objectBuffer->octTree, GO);
	}	

	GObject_FreeMembers(objID);
	MemoryPool_ReleaseID(objectBuffer->objectPool, objID);
}

///
//Retrieves an object given it's objectPool memory unit ID
//
//Parameters:
//	objID: The memory unit ID of the desired object in the objectPool
//
//Returns:
//	A pointer to the desired object
GObject* ObjectManager_LookupObject(unsigned int objID)
{
	return (GObject*)MemoryPool_RequestAddress(objectBuffer->objectPool, objID);
}

//TODO: Remove 4


///
//Adds an object to collection of objects managed by the Object Manager
//
//Parameters:
//	obj: The object to add
void ObjectManager_AddObject(GObject* obj)
{
	LinkedList_Append(objectBuffer->gameObjects, obj);
	if(obj->collider != NULL)
	{
		//Add the object
		OctTree_AddAndLog(objectBuffer->octTree, obj);
	}
}



///
//Removes an object from the collection of objects managed by the Object Manager
//Upon removal the Object Manager will NOT deallocate memory being used by the object!!!
//
//Parameters:
//	obj: The object to remove
void ObjectManager_RemoveObject(GObject* obj)
{
	LinkedList_RemoveValue(objectBuffer->gameObjects, obj);
	if(obj->collider != NULL)
	{
		OctTree_RemoveAndUnLog(objectBuffer->octTree, obj);
	}
}



///
//Removes an object from the collection of objects being managed by the object manager.
//Upon removal the object manager will deallocate memory being used by the object.
//
//Parameters:
//	obj: the object to remove and free
void ObjectManager_DeleteObject(GObject* obj)
{
	ObjectManager_RemoveObject(obj);
	GObject_Free(obj);
}



///
//Queues an object to be deleted at the end of the current update cycle
//
//Parameters:
//	obj: A pointer to the object to queue the deletion of
void ObjectManager_QueueDelete(GObject* obj)
{

	LinkedList_Append(objectBuffer->toDelete, obj);
}


///
//Allocates a new Object buffer
//
//Returns:
//	Returns a pointer to a newly allocated Object Buffer
static ObjectBuffer* ObjectManager_AllocateBuffer(void)
{
	ObjectBuffer* buffer = (ObjectBuffer*)malloc(sizeof(ObjectBuffer));
	return buffer;
}

///
//Initializes an Object Buffer
//
//Parameters:
//	buffer: The object buffer to initialize
static void ObjectManager_InitializeBuffer(ObjectBuffer* buffer)
{
	buffer->toDelete = LinkedList_Allocate();
	LinkedList_Initialize(buffer->toDelete);

	//buffer->gameObjects = LinkedList_Allocate();
	//LinkedList_Initialize(buffer->gameObjects);

	buffer->octTree = OctTree_Allocate();
	OctTree_Initialize(buffer->octTree, -100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f);

	buffer->objectPool = MemoryPool_Allocate();
	MemoryPool_Initialize(buffer->objectPool, sizeof(GObject));
}

///
//Frees resources being used by an Object Buffer
//
//Parameters:
//	buffer: The object buffer to free
static void ObjectManager_FreeBuffer(ObjectBuffer* buffer)
{
	//Free the oct tree
	OctTree_Free(buffer->octTree);

	//Delete all Objects being held in the object buffer
	//struct LinkedList_Node* current = buffer->gameObjects->head;

	//while (current != NULL)
	//{
	for(unsigned int i = 0; i < buffer->objectPool->pool->capacity; i++)
	{
		GObject* obj = (GObject*)MemoryPool_RequestAddress(buffer->objectPool, i);
		if(obj->frameOfReference != NULL)
		{
			GObject_FreeMembers(i);
		}
		//current = current->next;
	}

	//Now remove the list
	//LinkedList_Free(buffer->gameObjects);

	LinkedList_Free(buffer->toDelete);

	MemoryPool_Free(buffer->objectPool);
}
