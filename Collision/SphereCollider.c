#include "SphereCollider.h"

#include <stdlib.h>
#include <math.h>

#include "../Manager/AssetManager.h"
#include "../Manager/CollisionManager.h"

#include "Collider.h"

///
//Allocates memory for a new sphere collider
//
//Returns:
//	Pointer to a newly allocated sphere collider
int SphereCollider_AllocateData()
{

	//struct ColliderData_Sphere* colliderData = (struct ColliderData_Sphere*)malloc(sizeof(struct ColliderData_Sphere));
	int id = MemoryPool_RequestID(collisionBuffer->sphereData);
	int wID = MemoryPool_RequestID(collisionBuffer->worldSphereData);
	if(id != wID)
	{
		printf("Determinism error: SphereCollider_AllocateData.\nid:\t%d\trID:\t%d\n", id, wID);
	}
	return id;
}

///
//Initializes a sphere collider
//
//Parameters:
//	collider: The collider being initialized
//	rad: The radius of the collider to initialize
//	offset: A pointer to a vector to copy as the center of the sphere collider in object space
void SphereCollider_Initialize(Collider* collider, float rad, const Vector* offset)
{
	//Initialize collider
	Collider_Initialize(collider, COLLIDER_SPHERE, AssetManager_LookupMesh("Sphere"));

	//Allocate data
	collider->data->sphereDataID = SphereCollider_AllocateData();
	//Initialize data
	struct ColliderData_Sphere* sphereData = MemoryPool_RequestAddress(collisionBuffer->sphereData, collider->data->sphereDataID);
	
	sphereData->x = offset->components[0];
	sphereData->y = offset->components[1];
	sphereData->z = offset->components[2];
	sphereData->radius = rad;	
}

///
//Frees a sphere collider data set
//
//Parameters:
//	colliderDataID: The ID of the memory unit containing the sphere collider data
void SphereCollider_FreeData(int colliderDataID)
{
	//free(colliderData);
	MemoryPool_ReleaseID(collisionBuffer->sphereData, colliderDataID);
	MemoryPool_ReleaseID(collisionBuffer->worldSphereData, colliderDataID);
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

///
//Updates the x, y, and z position of the sphere to match the position
//of the given frame of reference
//
//Parameters:
//	sphereDataID: the ID of the sphere data to update
//	frame: A pointer to the frame of reference to update with
void SphereCollider_Update(const unsigned int sphereDataID, const FrameOfReference* frame)
{
	struct ColliderData_Sphere* sphereData = MemoryPool_RequestAddress(collisionBuffer->sphereData, sphereDataID);
	struct ColliderData_Sphere* worldSphereData = MemoryPool_RequestAddress(collisionBuffer->worldSphereData, sphereDataID);

	Vector worldSphere;			//Points to [worldSphereData.x, worldSphereData.y, worldSphereData.z]
	worldSphere.dimension = 3;
	worldSphere.components = &worldSphereData->x;

	worldSphereData->x = sphereData->x;
	worldSphereData->y = sphereData->y;
	worldSphereData->z = sphereData->z;

	Matrix_TransformVector(frame->rotation, &worldSphere);
	Matrix_TransformVector(frame->scale, &worldSphere);


	worldSphereData->x += frame->position->components[0];
	worldSphereData->y += frame->position->components[1];
	worldSphereData->z += frame->position->components[2];

	worldSphereData->radius = SphereCollider_GetScaledRadius(sphereData, frame);
}
