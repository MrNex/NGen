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
	float radius;
};

///
//Allocates memory for a new sphere collider data set
//
//Returns:
//	Pointer to a newly allocated sphere collider
struct ColliderData_Sphere* SphereCollider_AllocateData();

///
//Initializes a sphere collider
//
//Parameters:
//	collider: The collider being initialized
//	rad: The radius of the collider to initialize
//TODO:
//	centroid: A pointer to a vector to copy as the centroid of the sphere collider
void SphereCollider_Initialize(struct Collider* collider, const float rad);

///
//Frees a sphere collider data set
//
//Parameters:
//	colliderData: A pointer to the sphere collider data to free
void SphereCollider_FreeData(struct ColliderData_Sphere* colliderData);

///
//Gets the sphere radius scaled by the maximum scale of a frame of reference
//
//Parameters:
//	Collider: The collider to get the scaled radius of
//	FoR: The frame of reference to base scaling off of
float SphereCollider_GetScaledRadius(const struct ColliderData_Sphere* colliderData, const FrameOfReference* FoR);

#endif
