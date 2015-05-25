#include "Collider.h"

#include <stdlib.h>

///
//Gets the a pointer to the collider_Initialize function
//
//Returns:
//	Pointer to Collider_Initialize function
InitializerPtr Collider_GetInitializer(void)
{
	return initPtr;
}

///
//Destroys the only reachable pointer to the collider_Initialize function
//This will prevent any other files from gaining access to it that have not already gotten access.
void Collider_DestroyInitializerPtr(void)
{
	initPtr = 0;
}

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
		SphereCollider_FreeData(collider->data->sphereData);
		break;
	case COLLIDER_AABB:
		AABBCollider_FreeData(collider->data->AABBData);
		break;
	case COLLIDER_CONVEXHULL:
		ConvexHullCollider_FreeData(collider->data->convexHullData);
		break;
	}

	LinkedList_Free(collider->currentCollisions);

	//Free the color matrix of the collider
	Matrix_Free(collider->colorMatrix);

	//Free Collider data & Collider itself
	free(collider->data);
	free(collider);
}

