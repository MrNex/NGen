#include "AABBCollider.h"

#include <stdlib.h>

#include "../Manager/CollisionManager.h"
#include "../Manager/AssetManager.h"

#include "Collider.h"

///
//Allocates memory for a new Axis Aligned Bounding Box collider data
//
//returns:
//	ID of newly allocated uninitialized axis aligned bounding box collider data
int AABBCollider_AllocateData()
{
	//struct ColliderData_AABB* colliderData = (struct ColliderData_AABB*)malloc(sizeof(struct ColliderData_AABB));
	//return colliderData;
	int id = MemoryPool_RequestID(collisionBuffer->aabbData);
	int wID = MemoryPool_RequestID(collisionBuffer->worldAABBData);
	if(id != wID)
	{
		printf("Determinism error: AABBCollider_AllocateData.\nid:\t%d\trID:\t%d\n", id, wID);

	}

	return id;
	
}

///
//Initializes an AABB collider's data set
//
//Parameters:
//	AABB: The axis aligned bounding box data set being initialized
//	min: The minimum xyz point on the AABB in object space
//	max: The maximum xyz point on the AABB in object space
void AABBCollider_InitializeData(struct ColliderData_AABB* AABB, const float min[3], const float max[3])
{
	memcpy(AABB->min, min, 3 * sizeof(float));
	memcpy(AABB->max, max, 3 * sizeof(float));
}

///
//Initializes an axis aligned bounding box collider
//
//parameters:
//	collider: The collider to initialize
//	min: The minimum XYZ point on the AABB in object space
//	max: The maximum XYZ point on the AABB in object space
void AABBCollider_Initialize(struct Collider* collider, const float min[3], const float max[3])
{
	//Initialize the collider
	Collider_Initialize(collider, COLLIDER_AABB, AssetManager_LookupMesh("Cube"));

	//Allocate the datafor collider
	collider->data->AABBDataID = AABBCollider_AllocateData();
	struct ColliderData_AABB* aabb = Collider_GetColliderData(collider);

	//Initialize the data for collider
	AABBCollider_InitializeData(aabb, min, max);
}

///
//Initializes an axis aligned bounding box collider fit to a given mesh
//
//Parameters:
//	collider: The collider to initialize
//	mesh: The mesh to base the AABB collider's dimensions off of
void AABBCollider_InitializeFromMesh(Collider* collider, const Mesh* mesh)
{
	Vector centroid;
	Vector_INIT_ON_STACK(centroid, 3);

	Mesh_CalculateCentroid(&centroid, mesh);

	Vector dimensions;
	Vector_INIT_ON_STACK(dimensions, 3);

	Mesh_CalculateMaxDimensions(&dimensions, mesh, &centroid);

	const float min[3] = 
	{
		centroid.components[0] - 0.5f * dimensions.components[0],
		centroid.components[1] - 0.5f * dimensions.components[1],
		centroid.components[2] - 0.5f * dimensions.components[2]
	};

	const float max[3] =
	{
		centroid.components[0] - 0.5f * dimensions.components[0],
		centroid.components[1] - 0.5f * dimensions.components[1],
		centroid.components[2] - 0.5f * dimensions.components[2]
	};

	AABBCollider_Initialize(collider, min, max);
}

///
//Frees an axis aligned collider data set
//
//Parameters:
//	colliderDataID: The ID of the memoryunit containing the AABB collider data
void AABBCollider_FreeData(int colliderDataID)
{
	MemoryPool_ReleaseID(collisionBuffer->aabbData, colliderDataID);
	MemoryPool_ReleaseID(collisionBuffer->worldAABBData, colliderDataID);
}

///
//Gets the dimensions of an AABB scaled by the dimensions of a frame of reference
//
//Parameters:
//	dest: A pointer to a AABB Collider data set to store the scaled dimensions
//	colliderData: A pointer to the un-scaled AABB Collider data set
//	FoR: A pointer to the frame of reference by which to scale the collider data set
void AABBCollider_GetScaledDimensions(struct ColliderData_AABB* dest, const struct ColliderData_AABB* colliderData, const FrameOfReference* FoR)
{
	//dest->width = colliderData->width * Matrix_GetIndex(FoR->scale, 0, 0);
	//dest->height = colliderData->height * Matrix_GetIndex(FoR->scale, 1, 1);
	//dest->depth = colliderData->depth * Matrix_GetIndex(FoR->scale, 2, 2);

	dest->min[0] = colliderData->min[0] * Matrix_GetIndex(FoR->scale, 0, 0);
	dest->min[1] = colliderData->min[1] * Matrix_GetIndex(FoR->scale, 1, 1);
	dest->min[2] = colliderData->min[2] * Matrix_GetIndex(FoR->scale, 2, 2);

	dest->max[0] = colliderData->max[0] * Matrix_GetIndex(FoR->scale, 0, 0);
	dest->max[1] = colliderData->max[1] * Matrix_GetIndex(FoR->scale, 1, 1);
	dest->max[2] = colliderData->max[2] * Matrix_GetIndex(FoR->scale, 2, 2);
}

///
//Gets an AABB oriented in world space
//
//Parameters:
//	dest: A pointer to the destination to store the AABB in world space
//	colliderData: A pointer to the AABB toorient in worldspace
//	frame: A pointer to the frame of reference with which to orient the AABB
void AABBCollider_GetWorldAABB(struct ColliderData_AABB* dest, const struct ColliderData_AABB* colliderData, const FrameOfReference* frame)
{
	Vector min, max;
	min.dimension = max.dimension = 3;
	min.components = (float*)colliderData->min;
	max.components = (float*)colliderData->max;

	//Note: dimensions will actually hold the half dimensions.
	Vector dimensions;
	Vector_INIT_ON_STACK(dimensions, 3);

	Vector_Subtract(&dimensions, &max, &min);
	Vector_Scale(&dimensions, 0.5f);

	Vector centroid;
	Vector_INIT_ON_STACK(centroid, 3);

	Vector_Add(&centroid, &min, &dimensions);

	Matrix_TransformVector(frame->rotation, &centroid);
	Matrix_TransformVector(frame->scale, &centroid);
	Vector_Increment(&centroid, frame->position);

	Vector destVec;
       	destVec.dimension = 3;
	destVec.components = dest->min;

	Vector_Subtract(&destVec, &centroid, &dimensions);

	destVec.components = dest->max;

	Vector_Add(&destVec, &centroid, &dimensions);
}

///
//Takes an AABB Collider and represents it as a Convex Hull Collider
//
//Parameters:
//	dest:A pointer to an initialized & allocated convex hull collider data to store the representation
//	srcCollider: A pointer to an AABB collider data set to be represented as a convex hull collider
void AABBCollider_ToConvexHullCollider(struct ColliderData_ConvexHull* dest, const struct ColliderData_AABB* srcCollider)
{
	//Get points of AABB in modelSpace
	//1) Right Bottom Front
	Vector* point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->max[0];
	point->components[1] = srcCollider->min[1];
	point->components[2] = srcCollider->max[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//2) Righ bottom back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->max[0];
	point->components[1] = srcCollider->min[1];
	point->components[2] = srcCollider->min[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//3) Left bottom back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->min[0];
	point->components[1] = srcCollider->min[1];
	point->components[2] = srcCollider->min[2];;

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//4) Left bottom front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->min[0];
	point->components[1] = srcCollider->min[1];
	point->components[2] = srcCollider->max[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//5) Right Top front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->max[0];
	point->components[1] = srcCollider->max[1];
	point->components[2] = srcCollider->max[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//6) Right Top Back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->max[0];
	point->components[1] = srcCollider->max[1];
	point->components[2] = srcCollider->min[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//7) Left Top Back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->min[0];
	point->components[1] = srcCollider->max[1];
	point->components[2] = srcCollider->min[2];

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//8) Left Top Front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->min[0];
	point->components[1] = srcCollider->max[1];
	point->components[2] = srcCollider->max[2];;

	//Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	/*
	//Get axes of AABB
	//Right/Left side
	Vector* axis = Vector_Allocate();
	Vector_Initialize(axis, 3);

	Vector_Copy(axis, &Vector_E1);

	ConvexHullCollider_AddAxis(dest, axis);

	//Top/Bottom side
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);

	Vector_Copy(axis, &Vector_E2);

	ConvexHullCollider_AddAxis(dest, axis);

	//Front/Back side
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);

	Vector_Copy(axis, &Vector_E3);
	
	ConvexHullCollider_AddAxis(dest, axis);
	*/

	struct ConvexHullCollider_Face* face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	Vector* edge = Vector_Allocate();
	Vector_Initialize(edge, 3);


	face->normal->components[0] = 1.0f;
	Vector_Copy(edge, face->normal);

	int index = 0;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 1;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);

	//ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddFace(dest, face);
	ConvexHullCollider_AddEdge(dest, edge);

	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[0] = -1.0f;

	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(dest, face);


	//Top/Bottom face
	//axis = Vector_Allocate();
	//Vector_Initialize(axis, 3);
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);


	//axis->components[1] = 1.0f;
	face->normal->components[1] = 1.0f;
	//Vector_Copy(edge, axis);
	Vector_Copy(edge, face->normal);

	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	//ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddFace(dest, face);
	ConvexHullCollider_AddEdge(dest, edge);

	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[1] = -1.0f;

	index = 1;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(dest, face);

	//Front/Back face
	//axis = Vector_Allocate();
	//Vector_Initialize(axis, 3);
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);


	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	//axis->components[2] = 1.0f;
	face->normal->components[2] = 1.0f;
	//Vector_Copy(edge, axis);
	Vector_Copy(edge, face->normal);

	index = 0;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	//ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddFace(dest, face);
	ConvexHullCollider_AddEdge(dest, edge);

	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[2] = -1.0f;

	index = 1;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(dest, face);

}

///
//Updates the worldspace data of this collider to match that of the given frame of reference
//
//Parameters:
//	colliderDataID: The ID of the collider
//	frame: A pointer to the frame of reference with which to orient this collider
void AABBCollider_Update(const unsigned int colliderDataID, FrameOfReference* frame)
{
	struct ColliderData_AABB* aabbData = MemoryPool_RequestAddress(collisionBuffer->aabbData, colliderDataID);
	struct ColliderData_AABB* worldAABBData = MemoryPool_RequestAddress(collisionBuffer->worldAABBData, colliderDataID);


	Vector centroid;
	Vector_INIT_ON_STACK(centroid, 3);
	Vector_CopyArray(centroid.components, aabbData->min, 3);
	Vector_IncrementArray(centroid.components, aabbData->max, 3);
	Vector_Scale(&centroid, 0.5f);
	
	FrameOfReference_TransformVector(frame, &centroid);

	Matrix_GetProductVectorArray(worldAABBData->max, frame->scale->components, aabbData->max, 3, 3);
	Matrix_GetProductVectorArray(worldAABBData->min, frame->scale->components, aabbData->min, 3, 3);

	Vector_IncrementArray(worldAABBData->max, centroid.components, 3);
	Vector_IncrementArray(worldAABBData->min, centroid.components, 3);

}
