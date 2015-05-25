#ifndef COLLIDER_H
#define COLLIDER_H

#include "ColliderEnum.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "ConvexHullCollider.h"

//Dictates the type of a collider
/*
enum ColliderType
{
	COLLIDER_SPHERE,
	COLLIDER_AABB,
	COLLIDER_CONVEXHULL
};
*/

//union for the data different colliders will provide
union ColliderData
{
	struct ColliderData_Sphere* sphereData;
	struct ColliderData_AABB* AABBData;
	struct ColliderData_ConvexHull* convexHullData;
};

//Defines a collider
typedef struct Collider
{
	ColliderType type;				//Type of collider
	union ColliderData* data;		//Data of collider

	LinkedList* currentCollisions;	//List of all collisions which occurred with this collider last frame

	unsigned char debug;			//Is collider in debug mode?
	Mesh* representation;			//ptr to Mesh representation of collider
	Matrix* colorMatrix;			//Matrix to control color of mesh representation in debug mode
} Collider;

typedef void(*InitializerPtr)(Collider*, ColliderType, Mesh* rep);

///
//Allocates memory for a Collider
//
//Returns:
//	Pointer to newly allocated uninitialized collider
Collider* Collider_Allocate();

///
//Initializes general collider information
//
//Parameters:
//	collider: THe collider to initialize
//	type: The type of the collider being initialized
//	rep: A pointer to a mesh which can represent this collider in debug mode
static void Collider_Initialize(Collider* collider, ColliderType type, Mesh* rep)
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

//Pointer to the above function, only valid until Collision Manager is initialized!
static InitializerPtr initPtr = &Collider_Initialize;

///
//Gets the above initializer pointer
//
//Returns:
//	Pointer to Collider_Initialize function
InitializerPtr Collider_GetInitializer(void);

///
//Destroys the above initializer pointer so no other files can access it!
void Collider_DestroyInitializerPtr(void);

///
//Frees memory allocated by a collider of any type
//
//Parameters:
//	collider: The collider to free
void Collider_Free(Collider* collider);

#endif
