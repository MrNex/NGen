#include "RayCollider.h"

#include <stdlib.h>

#include "Collider.h"

///
//Allocates memory for a new ray collider data set
//
//Returns:
//	Pointer to a newly allocated ray collider
struct ColliderData_Ray* RayCollider_AllocateData()
{
	return malloc(sizeof(struct ColliderData_Ray));
}

///
//Initializes a ray collider
//
//Parameters:
//	collider: A pointer to the collider to initialize as a ray collider
//	direction: A pointer to a vector to copy as the direction of the ray
//	position: A pointer to a vector to copy as the position of the ray
void RayCollider_Initialize(struct Collider* collider, Vector* direction, Vector* position)
{
	Collider_Initialize(collider, COLLIDER_RAY, NULL);

	collider->data->rayData = RayCollider_AllocateData();
	collider->data->rayData->position = Vector_Allocate();
	collider->data->rayData->direction = Vector_Allocate();

	Vector_Initialize(collider->data->rayData->position, 3);
	Vector_Initialize(collider->data->rayData->direction, 3);

	Vector_Copy(collider->data->rayData->position, position);
	Vector_Copy(collider->data->rayData->direction, direction);
	
}

///
//Frees a ray collider data set
//
//Parameters:
//	colliderDatA: A pointer to the ray collider data to free
void RayCollider_FreeData(struct ColliderData_Ray* colliderData)
{
	Vector_Free(colliderData->direction);
	Vector_Free(colliderData->position);
	free(colliderData);
}

///
//Calculates the position and direction of a ray in world space after being oriented by a frame of reference.
//
//Parameters:
//	worldRay: A pointer to a ray collider to store the position and direction of the ray in worldspace
//	ray: A pointer to the ray being oriented
//	rayFrame: A pointer to the frame of reference of the ray
void RayCollider_GetWorldRay(struct ColliderData_Ray* worldRay, struct ColliderData_Ray* ray, FrameOfReference* rayFrame)
{
	Matrix_GetProductVector(worldRay->position, rayFrame->rotation, ray->position);
	Matrix_TransformVector(rayFrame->scale, worldRay->position);

	Vector_Increment(worldRay->position, rayFrame->position);

	Matrix_GetProductVector(worldRay->direction, rayFrame->rotation, ray->direction);
}
