#include "SphereCollider.h"

#include <stdlib.h>
#include <math.h>

#include "../Manager/AssetManager.h"

#include "Collider.h"

///
//Allocates memory for a new sphere collider
//
//Returns:
//	Pointer to a newly allocated sphere collider
struct ColliderData_Sphere* SphereCollider_AllocateData()
{
	struct ColliderData_Sphere* colliderData = (struct ColliderData_Sphere*)malloc(sizeof(struct ColliderData_Sphere));
	return colliderData;
}

///
//Initializes a sphere collider
//
//Parameters:
//	collider: The collider being initialized
//	rad: The radius of the collider to initialize
//TODO:
//	centroid: A pointer to a vector to copy as the centroid of the sphere collider
void SphereCollider_Initialize(Collider* collider, float rad)
{
	//Initialize collider
	Collider_Initialize(collider, COLLIDER_SPHERE, AssetManager_LookupMesh("Sphere"));

	//Allocate data
	collider->data->sphereData = SphereCollider_AllocateData();
	//Initialize data
	collider->data->sphereData->radius = rad;
}

///
//Initializes a sphere collider as a deep copy of another
//This means any pointers will point to a Newly Allocated instance of identical memory
//Parameters:
//	copy: A pointer to an uninitialized collider to initialize as a deep copy
//	original: A pointer to a sphere collider to deep copy
void SphereCollider_InitializeDeepCopy(struct Collider* copy, struct Collider* original)
{
	SphereCollider_Initialize(copy, original->data->sphereData->radius);
}

///
//Frees a sphere collider data set
//
//Parameters:
//	colliderData: A pointer to the sphere collider data to free
void SphereCollider_FreeData(struct ColliderData_Sphere* colliderData)
{
	free(colliderData);
}

///
//Gets the sphere radius scaled by the maximum scale of a frame of reference
//
//Parameters:
//	Collider: The collider to get the scaled radius of
//	FoR: The frame of reference to base scaling off of
float SphereCollider_GetScaledRadius(const struct ColliderData_Sphere* colliderData, const FrameOfReference* FoR)
{
	//Find largest axis scale value in each objects Frame of reference
	float objMaxScale = 0.0f;
	float assignor = 0.0f;
	for(int i = 0; i < 3; i++)
	{
		assignor = fabs(Matrix_GetIndex(FoR->scale, i, i));
		if(assignor > objMaxScale) objMaxScale = assignor;
	}
	//And scale collider radius by it
	return objMaxScale * colliderData->radius;
}
