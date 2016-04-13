#ifndef RAYCOLLIDER_H
#define RAYCOLLIDER_H

#include "ColliderEnum.h"
#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"

#define ColliderData_Ray_INIT_ON_STACK( ray ) \
	Vector dir##ray; \
	Vector pos##ray; \
	Vector_INIT_ON_STACK( dir##ray , 3 ); \
	Vector_INIT_ON_STACK( pos##ray , 3 ); \
	ray.direction = &dir##ray; \
	ray.position = &pos##ray;



//Forward declaration of collider type to avoid circular dependency

struct Collider;

struct ColliderData_Ray
{
	Vector* direction;
	Vector* position;
};

///
//Allocates memory for a new ray collider data set
//
//Returns:
//	Pointer to a newly allocated ray collider
struct ColliderData_Ray* RayCollider_AllocateData();

///
//Initializes a ray collider
//
//Parameters:
//	collider: A pointer to the collider to initialize as a ray collider
//	direction: A pointer to a vector to copy as the direction of the ray
//	position: A pointer to a vector to copy as the position of the ray
void RayCollider_Initialize(struct Collider* collider, Vector* direction, Vector* position);

///
//Frees a ray collider data set
//
//Parameters:
//	colliderDatA: A pointer to the ray collider data to free
void RayCollider_FreeData(struct ColliderData_Ray* colliderData);

///
//Calculates the position and direction of a ray in world space after being oriented by a frame of reference.
//
//Parameters:
//	worldRay: A pointer to a ray collider to store the position and direction of the ray in worldspace
//	ray: A pointer to the ray being oriented
//	rayFrame: A pointer to the frame of reference of the ray
void RayCollider_GetWorldRay(struct ColliderData_Ray* worldRay, struct ColliderData_Ray* ray, FrameOfReference* rayFrame);

#endif
