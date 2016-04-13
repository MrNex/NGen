
#include "Collider.h"
#include "ColliderEnum.h"

#include <stdlib.h>

#include "../Manager/CollisionManager.h"

///
//Declarations
///


///
//Allocates memory for a Collider
//
//Returns:
//	Pointer to newly allocated uninitialized collider
Collider* Collider_Allocate()
{
	Collider* collider = (Collider*)malloc(sizeof(Collider));
	return collider;
}

///
//Initializes general collider information
//
//Parameters:
//	collider: THe collider to initialize
//	type: The type of the collider being initialized
//	rep: A pointer to a mesh which can represent this collider in debug mode
void Collider_Initialize(Collider* collider, ColliderType type, Mesh* rep)
{
	collider->data = (union ColliderData*)malloc(sizeof(union ColliderData));
	collider->type = type;

	collider->currentCollisions = LinkedList_Allocate();
	LinkedList_Initialize(collider->currentCollisions);

	//Initialize with debug mode on & setup debug settings
	collider->debug = 0;
	collider->representation = rep;

	collider->colorMatrix = Matrix_Allocate();
	Matrix_Initialize(collider->colorMatrix, 4, 4);

	//Set color matrix to green
	*Matrix_Index(collider->colorMatrix, 0, 0) = 0.0f;	//Red
	*Matrix_Index(collider->colorMatrix, 1, 1) = 1.0f;	//Green
	*Matrix_Index(collider->colorMatrix, 2, 2) = 0.0f;	//Blue
}

///
//Frees memory allocated by a collider of any type
//
//Parameters:
//	collider: The collider to free
void Collider_Free(Collider* collider)
{
	//Free Collider Type Specific Data
	switch(collider->type)
	{
	case COLLIDER_SPHERE:
		SphereCollider_FreeData(collider->data->sphereDataID);
		break;
	case COLLIDER_AABB:
		AABBCollider_FreeData(collider->data->AABBDataID);
		break;
	case COLLIDER_CONVEXHULL:
		ConvexHullCollider_FreeData(collider->data->convexHullData);
		break;
	case COLLIDER_RAY:
		RayCollider_FreeData(collider->data->rayData);
		break;
	}

	LinkedList_Free(collider->currentCollisions);

	//Free the color matrix of the collider
	Matrix_Free(collider->colorMatrix);

	//Free Collider data & Collider itself
	free(collider->data);
	free(collider);
}

///
//Gets the collider data for a given collider
//
//Parameters:
//	collider: A pointer to the collider to get the data from
//
//Returns:
//	A pointer to the data for this collider
void* Collider_GetColliderData(Collider* collider)
{
	switch(collider->type)
	{
	case COLLIDER_SPHERE:
		return MemoryPool_RequestAddress(collisionBuffer->sphereData, collider->data->sphereDataID);
		break;
	case COLLIDER_AABB:
		return MemoryPool_RequestAddress(collisionBuffer->aabbData, collider->data->AABBDataID);
		break;
	case COLLIDER_CONVEXHULL:
		return collider->data->convexHullData;
		break;
	case COLLIDER_RAY:
		return collider->data->rayData;
		break;
	}
	return NULL;
}

///
//Gets the collider data for a given collider oriented in world space
//
//Parameters:
//	collider: A pointer to the collider to retrieve the world space collider data for
//
//Returns:
//	A pointer to the data for this collider which is oriented in world space
void* Collider_GetColliderDataWorldSpace(Collider* collider)
{
	MemoryPool* pool = CollisionManager_RetrieveMemoryPool(collider->type, 1);
	return MemoryPool_RequestAddress(pool, collider->data->sphereDataID);
}

///
//Updates a collider to mimic the orientation of a given frame of reference
//
//Parameters:
//	collider: A pointer to the collider to update
//	frame: A pointer to the frame of reference to update with
void Collider_Update(Collider* collider, FrameOfReference* frame)
{
	switch(collider->type)
	{
		case COLLIDER_SPHERE:
			SphereCollider_Update(collider->data->sphereDataID, frame);
			break;
		case COLLIDER_AABB:
			AABBCollider_Update(collider->data->AABBDataID, frame);
			break;
		default:
			break;
	}
}
