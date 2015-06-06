#include "Collider.h"

#include <stdlib.h>

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

