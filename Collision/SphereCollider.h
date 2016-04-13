#ifndef SPHERECOLLIDER_H
#define SPHERECOLLIDER_H

#include "ColliderEnum.h"
#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"


//Forward declaration of Collider to avoid circular dependency
struct Collider;
//typedef enum ColliderType;



struct ColliderData_Sphere
{
	float x, y, z;
	float radius;
};

///
//Allocates memory for a new sphere collider data set
//
//Returns:
//	ID in collisionBuffer of a newly allocated sphere collider
int SphereCollider_AllocateData();

///
//Initializes a sphere collider
//
//Parameters:
//	collider: The collider being initialized
//	rad: The radius of the collider to initialize
//	offset: A pointer to a vector to copy as the center of the sphere collider in object space
void SphereCollider_Initialize(struct Collider* collider, float rad, const Vector* offset);

///
//Initializes a sphere collider as a deep copy of another
//This means any pointers will point to a Newly Allocated instance of identical memory
//Parameters:
//	copy: A pointer to an uninitialized collider to initialize as a deep copy
//	original: A pointer to a sphere collider to deep copy
void SphereCollider_InitializeDeepCopy(struct Collider* copy, struct Collider* original);

///
//Frees a sphere collider data set
//
//Parameters:
//	colliderDataID: The ID of the memory unit containing the sphere collider data
void SphereCollider_FreeData(int colliderDataID);

///
//Gets the sphere radius scaled by the maximum scale of a frame of reference
//
//Parameters:
//	Collider: The collider to get the scaled radius of
//	FoR: The frame of reference to base scaling off of
float SphereCollider_GetScaledRadius(const struct ColliderData_Sphere* colliderData, const FrameOfReference* FoR);

///
//Updates the x, y, and z position of the sphere to match the position
//of the given frame of reference
//
//Parameters:
//	sphereDataID: the ID of the sphere data to update
//	frame: A pointer to the frame of reference to update with
void SphereCollider_Update(const unsigned int sphereDataID, const FrameOfReference* frame);

#endif
