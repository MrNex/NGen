#ifndef COLLIDER_H
#define COLLIDER_H

#include "ColliderEnum.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "ConvexHullCollider.h"
#include "RayCollider.h"

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
	unsigned int sphereDataID;
	//struct ColliderData_Sphere* sphereDat
	unsigned int AABBDataID;
	struct ColliderData_ConvexHull* convexHullData;
	struct ColliderData_Ray* rayData;
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
void Collider_Initialize(Collider* collider, ColliderType type, Mesh* rep);

///
//Frees memory allocated by a collider of any type
//
//Parameters:
//	collider: The collider to free
void Collider_Free(Collider* collider);

///
//Gets the collider data for a given collider
//
//Parameters:
//	collider: A pointer to the collider to get the data from
//
//Returns:
//	A pointer to the data for this collider
void* Collider_GetColliderData(Collider* collider);

///
//Gets the collider data for a given collider oriented in world space
//
//Parameters:
//	collider: A pointer to the collider to retrieve the world space collider data for
//
//Returns:
//	A pointer to the data for this collider which is oriented in world space
void* Collider_GetColliderDataWorldSpace(Collider* collider);

///
//Updates a collider to mimic the orientation of a given frame of reference
//
//Parameters:
//	collider: A pointer to the collider to update
//	frame: A pointer to the frame of reference to update with
void Collider_Update(Collider* collider, FrameOfReference* frame);

#endif
