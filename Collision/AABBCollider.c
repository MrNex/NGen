#include "AABBCollider.h"

#include <stdlib.h>

#include "../Manager/AssetManager.h"

#include "Collider.h"

///
//Allocates memory for a new Axis Aligned Bounding Box collider data
//
//returns:
//	Pointer to newly allocated uninitialized axis aligned bounding box collider data
struct ColliderData_AABB* AABBCollider_AllocateData()
{
	struct ColliderData_AABB* colliderData = (struct ColliderData_AABB*)malloc(sizeof(struct ColliderData_AABB));
	return colliderData;
}

///
//Initializes an AABB collider's data set
//
//Parameters:
//	AABB: The axis aligned bounding box data set being initialized
//	width: The width of the AABB
//	height: The height of the AABB
//	depth: The depth of the AABB
//	centroid: A pointer to a vector to copy as the centroid of the AABB
void AABBCollider_InitializeData(struct ColliderData_AABB* AABB, const float width, const float height, const float depth, const Vector* centroid)
{
	AABB->centroid = Vector_Allocate();
	Vector_Initialize(AABB->centroid, 3);

	AABB->width = width;
	AABB->height = height;
	AABB->depth = depth;

	Vector_Copy(AABB->centroid, centroid);
}

///
//Initializes an axis aligned bounding box collider
//
//Parameters:
//	collider: The collider to initialize
//	width: The width of the bounding box
//	height: The height of the bounding box
//	depth: The depth of the bounding box
void AABBCollider_Initialize(Collider* collider, float width, float height, float depth, const Vector* centroid)
{
	//Initialize the collider
	Collider_Initialize(collider, COLLIDER_AABB, AssetManager_LookupMesh("Cube"));

	//Allocate the datafor collider
	collider->data->AABBData = AABBCollider_AllocateData();
	//Initialize the data for collider
	AABBCollider_InitializeData(collider->data->AABBData, width, height, depth, centroid);
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

	AABBCollider_Initialize(collider, dimensions.components[0], dimensions.components[1], dimensions.components[2], &centroid);
}

///
//Frees an axis aligned collider data set
//
//Parameters:
//	colliderData: A pointer to the axis aligned bounding box collider data to free
void AABBCollider_FreeData(struct ColliderData_AABB* colliderData)
{
	Vector_Free(colliderData->centroid);
	free(colliderData);
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
	dest->width = colliderData->width * Matrix_GetIndex(FoR->scale, 0, 0);
	dest->height = colliderData->height * Matrix_GetIndex(FoR->scale, 1, 1);
	dest->depth = colliderData->depth * Matrix_GetIndex(FoR->scale, 2, 2);
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
	
	point->components[0] = srcCollider->width / 2.0f;
	point->components[1] = srcCollider->height / -2.0f;
	point->components[2] = srcCollider->depth / 2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//2) Righ bottom back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / 2.0f;
	point->components[1] = srcCollider->height / -2.0f;
	point->components[2] = srcCollider->depth / -2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//3) Left bottom back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / -2.0f;
	point->components[1] = srcCollider->height / -2.0f;
	point->components[2] = srcCollider->depth / -2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//4) Left bottom front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / -2.0f;
	point->components[1] = srcCollider->height / -2.0f;
	point->components[2] = srcCollider->depth / 2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//5) Right Top front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / 2.0f;
	point->components[1] = srcCollider->height / 2.0f;
	point->components[2] = srcCollider->depth / 2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//6) Right Top Back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / 2.0f;
	point->components[1] = srcCollider->height / 2.0f;
	point->components[2] = srcCollider->depth / -2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//7) Left Top Back
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / -2.0f;
	point->components[1] = srcCollider->height / 2.0f;
	point->components[2] = srcCollider->depth / -2.0f;

	Vector_Increment(point, srcCollider->centroid);

	//Add to convex hull
	ConvexHullCollider_AddPoint(dest, point);

	//8) Left Top Front
	point = Vector_Allocate();
	Vector_Initialize(point, 3);
	
	point->components[0] = srcCollider->width / -2.0f;
	point->components[1] = srcCollider->height / 2.0f;
	point->components[2] = srcCollider->depth / 2.0f;

	Vector_Increment(point, srcCollider->centroid);

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
