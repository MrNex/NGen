#include "ConvexHullCollider.h"

#include <stdlib.h>
#include <float.h>

#include "Collider.h"

#include "../Manager/AssetManager.h"

///
//Allocates memory for a new Convex Hull face
//
//Returns:
//	A pointer to a newly allocated, uninitialized, convex hull face
struct ConvexHullCollider_Face* ConvexHullCollider_AllocateFace()
{
	struct ConvexHullCollider_Face* face = (struct ConvexHullCollider_Face*)malloc(sizeof(struct ConvexHullCollider_Face));
	return face;
}

///
//Initializes a ConvexHull_Face
//
//Parameters:
//	face: A Pointer to the ConvexHull_Face to initialize
void ConvexHullCollider_InitializeFace(struct ConvexHullCollider_Face* face)
{
	face->normal = Vector_Allocate();
	Vector_Initialize(face->normal, 3);

	face->indicesOnFace = DynamicArray_Allocate();
	DynamicArray_Initialize(face->indicesOnFace, sizeof(unsigned int));	
}

///
//Frees memory allocated by a ConvexHull_Face
//
//Parameters:
//	face: The convex hull face to free
void ConvexHullCollider_FreeFace(struct ConvexHullCollider_Face* face)
{
	Vector_Free(face->normal);
	DynamicArray_Free(face->indicesOnFace);
	free(face);
}

///
//Allocates memory for a new convex hull collider data set
//
//Returns:
//	Pointer to a newly allocated convex hull collider data set
struct ColliderData_ConvexHull* ConvexHullCollider_AllocateData()
{
	struct ColliderData_ConvexHull* colliderData = (struct ColliderData_ConvexHull*)malloc(sizeof(struct ColliderData_ConvexHull));
	return colliderData;
}

///
//Initializes a convex hull data set
//
//Parameters:
//	convexData: THe convex Hull Data Set being initialized
void ConvexHullCollider_InitializeData(struct ColliderData_ConvexHull* convexHullData)
{
	convexHullData->points = DynamicArray_Allocate();
	DynamicArray_Initialize(convexHullData->points, sizeof(Vector*));

	convexHullData->faces = DynamicArray_Allocate();
	DynamicArray_Initialize(convexHullData->faces, sizeof(struct ConvexHull_Face*));

	convexHullData->edges = DynamicArray_Allocate();
	DynamicArray_Initialize(convexHullData->edges, sizeof(Vector*));
}

///
//Initializes a convex hull collider
//
//Parameters:
//	collider: The colider being initialized
void ConvexHullCollider_Initialize(Collider* collider)
{
	//TODO: Change to initialize from a mesh with a proper mesh representation
	//Initialize the collider
	Collider_Initialize(collider, COLLIDER_CONVEXHULL, AssetManager_LookupMesh("Cube"));

	//Allocate the collider data
	collider->data->convexHullData = ConvexHullCollider_AllocateData();

	//Initialize the collider data
	ConvexHullCollider_InitializeData(collider->data->convexHullData);
}

///
//Frees a convex hull collider data set
//
//Parameters:
//	colliderData: A pointer to the convex hull collider data to free
void ConvexHullCollider_FreeData(struct ColliderData_ConvexHull* colliderData)
{
	//Free all vectors in points, axes, and edges dynamic arrays, aswell as the dynamic arrays themselves
	for(unsigned int i = 0; i < colliderData->points->size; i++)
	{
		Vector* point = *(Vector**)DynamicArray_Index(colliderData->points, i);
		Vector_Free(point);
	}
	DynamicArray_Clear(colliderData->points);
	DynamicArray_Free(colliderData->points);

	for(unsigned int i = 0; i < colliderData->faces->size; i++)
	{
		struct ConvexHullCollider_Face* face = *(struct ConvexHullCollider_Face**)DynamicArray_Index(colliderData->faces, i);
		ConvexHullCollider_FreeFace(face);
	}
	DynamicArray_Clear(colliderData->faces);
	DynamicArray_Free(colliderData->faces);

	for(unsigned int i = 0; i < colliderData->edges->size; i++)
	{
		Vector* edge = *(Vector**)DynamicArray_Index(colliderData->edges, i);
		Vector_Free(edge);
	}
	DynamicArray_Clear(colliderData->edges);
	DynamicArray_Free(colliderData->edges);

	free(colliderData);
}

///
//Adds a point to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull having a point added
//	point: A pointer to a vector of dimension 3 representing the point to add
void ConvexHullCollider_AddPoint(struct ColliderData_ConvexHull* collider, const Vector* point)
{
	DynamicArray_Append(collider->points, (Vector**)&point);
}

///
//Adds a face to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull collider having a face added
//	face: A pointer to the face being added
void ConvexHullCollider_AddFace(struct ColliderData_ConvexHull* collider, const struct ConvexHullCollider_Face* face)
{
	DynamicArray_Append(collider->faces, (struct ConvexHullCollider_Face**)&face);
}

///
//Adds an edge to a convex hull collider
//
//Parameters:
//	Collider: A pointer to the convex hull collider having an axis added
//	axis: A pointer to a vector of dimension 3 representing the direction vector of an edge on the collider
void ConvexHullCollider_AddEdge(struct ColliderData_ConvexHull* collider, const Vector* edgeDirection)
{
	DynamicArray_Append(collider->edges, (Vector**)&edgeDirection);
}

///
//Makes a cube convex hull collider from a blank initialized convex hull collider
//Convex hull will be centered around (0, 0, 0) ranging from +- sideLength / 2
//
//Parameters:
//	collider: A pointer to a convex hull collider with no current points or axis to make into a cube
//	sideLength: The initial sidelength of the cube collider to make
void ConvexHullCollider_MakeCubeCollider(struct ColliderData_ConvexHull* collider, float sideLength)
{
	//First add points
	//0) Lower Right Front Corner
	Vector* point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//1) Lower Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//2) Lower Left Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//3) Lower Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//4) Upper Right Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//5) Upper Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//6) Upper LEft Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//7) Upper Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//Second add faces & edges
	//Right/Left face
	//Right
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
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Left
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[0] = -1.0f;

	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);


	//Top/Bottom face
	//Top
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	face->normal->components[1] = 1.0f;
	Vector_Copy(edge, face->normal);

	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Bottom
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[1] = -1.0f;

	index = 0;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 1;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);

	//Front/Back face
	//Front
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);


	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	face->normal->components[2] = 1.0f;
	Vector_Copy(edge, face->normal);

	index = 0;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Back
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

	ConvexHullCollider_AddFace(collider, face);

}

///
//Makes a rectangular convex hull collider from a blank initialized convex hull collider
//
//Parameters:
//	collider: A pointer to a convex hullcollider with no current points or axis to make into a rectangle
//	width: The width of the collider
//	Height: The height of the collider
//	depth: The depth of the collider
void ConvexHullCollider_MakeRectangularCollider(struct ColliderData_ConvexHull* collider, float width, float height, float depth)
{
	//First add points
	//1) Lower Right Front Corner
	Vector* point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = width / 2.0f;
	point->components[1] = -height / 2.0f;
	point->components[2] = depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//2) Lower Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = width / 2.0f;
	point->components[1] = -height / 2.0f;
	point->components[2] = -depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//3) Lower Left Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -width / 2.0f;
	point->components[1] = -height / 2.0f;
	point->components[2] = -depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//4) Lower Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -width / 2.0f;
	point->components[1] = -height / 2.0f;
	point->components[2] = depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//5) Upper Right Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = width / 2.0f;
	point->components[1] = height / 2.0f;
	point->components[2] = depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//6) Upper Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = width / 2.0f;
	point->components[1] = height / 2.0f;
	point->components[2] = -depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//7) Upper LEft Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -width / 2.0f;
	point->components[1] = height / 2.0f;
	point->components[2] = -depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//8) Upper Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -width / 2.0f;
	point->components[1] = height / 2.0f;
	point->components[2] = depth / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//add axes & edges


	//Right/Left face
	//Right
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

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Left
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

	ConvexHullCollider_AddFace(collider, face);


	//Top/Bottom face
	//Top
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	face->normal->components[1] = 1.0f;
	Vector_Copy(edge, face->normal);

	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Bottom
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

	ConvexHullCollider_AddFace(collider, face);

	//Front/Back face
	//Front
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);


	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	face->normal->components[2] = 1.0f;
	Vector_Copy(edge, face->normal);

	index = 0;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 3;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 4;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 7;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
	ConvexHullCollider_AddEdge(collider, edge);

	//Back
	face = ConvexHullCollider_AllocateFace();
	ConvexHullCollider_InitializeFace(face);

	face->normal->components[2] = -1.0f;

	index = 1;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 2;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 6;
	DynamicArray_Append(face->indicesOnFace, &index);
	index = 5;
	DynamicArray_Append(face->indicesOnFace, &index);

	ConvexHullCollider_AddFace(collider, face);
}

///
//Gets the points of the collider oriented in world space to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider points
//	collider: The collider of which to orient the points of
//	frame: The frame of reference with which to orient the points
void ConvexHullCollider_GetOrientedWorldPoints(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame)
{
	//Get the points oriented in model space
	ConvexHullCollider_GetOrientedModelPoints(dest, collider, frame);

	//Translate to world space
	for(unsigned int i = 0; i < collider->points->size; i++)
	{
		Vector_Increment(dest[i], frame->position);
	}

}

///
//Gets the points of the collider oriented in model space to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider points
//	collider: The collider of which to orient the points of
//	frame: A pointer to the frame of reference with which to orient the points
void ConvexHullCollider_GetOrientedModelPoints(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame)
{
	//Create a transformation matrix to rotate and scale the points
	Matrix trans;
	Matrix_INIT_ON_STACK(trans, 3, 3);
	Matrix_GetProductMatrix(&trans, frame->rotation, frame->scale);

	//Loop through the colliders points
	for(unsigned int i = 0; i < collider->points->size; i++)
	{
		Vector* currentPoint = *(Vector**)DynamicArray_Index(collider->points, i);
		//Rotate and scale each point
		Matrix_GetProductVector(dest[i], &trans, currentPoint);
	}
}

///
//Gets the axes of a convex hull collider oriented to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider axes
//	collider: A pointer to the collider of which to orient the axes of
//	frame: A pointer to the frame of reference with which to orient the axes
void ConvexHullCollider_GetOrientedAxes(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame)
{
	//Loop through axes
	for(unsigned int i = 0; i < collider->faces->size; i++)
	{
		Vector* currentAxis = (*(struct ConvexHullCollider_Face**)DynamicArray_Index(collider->faces, i))->normal;
		//Rotate each axis 
		Matrix_GetProductVector(dest[i], frame->rotation, currentAxis);
	}
}

///
//Gets the edges of a convex hull collider oriented to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider edges
//	collider: A pointer to the collider of which to orient the edges of
//	frame: A pointer to the frame of reference with which to orient the edges
void ConvexHullCollider_GetOrientedEdges(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame)
{
	//Loop through edges
	for(unsigned int i = 0; i < collider->edges->size; i++)
	{
		Vector* currentEdge = *(Vector**)DynamicArray_Index(collider->edges, i);
		//Rotate each edge 
		Matrix_GetProductVector(dest[i], frame->rotation, currentEdge);
	}
}

///
//Determines the subset set of a convex hull colliders points oriented to a given orientation furthest in a given direction.
//These points are returned in MODEL SPACE! The components of the vectors in the dynamic array POINT TO the components of the vectors in the model oriented points array!
//
//Parameters:
//	dest: A dynamic array to store the set of points in
//	collider: A pointer to a convex hull collider to get the subset of points from
//	modelOrientedPoints: An array of pointers to vectors representing the colliders points oriented into modelSpace
//	frame: The frame to orient the collider's points with
//	direction: The direction in which the desired set of points are furthest in.
void ConvexHullCollider_GetFurthestPoints(DynamicArray* dest, const struct ColliderData_ConvexHull* collider, const Vector** modelOrientedPoints, const Vector* direction)
{

	//This is a tolerance range within which we consider points just as far.
	static const float tolerance = 0.001f;

	//Find the points furthest in direction of relative MTV for convexHull1
	float currentMaxDistance = 0.0f;
	float currentDistance = 0.0f;

	//Loop through points
	for(unsigned int i = 0; i < collider->points->size; i++)
	{
		//Get the ith points distance in direction of MTV
		currentDistance = Vector_DotProduct(modelOrientedPoints[i], direction);

		//Check if this point is further than the current furthest
		if(currentDistance > currentMaxDistance)
		{
			//If we have already logged points as being the furthest, we must clear then
			if(dest->size != 0)
			{
				DynamicArray_Clear(dest);
			}

			//Log this point as the new furthest
			currentMaxDistance = currentDistance;
			DynamicArray_Append(dest, (void*)modelOrientedPoints[i]);
		}
		//If it's not further, it may be just as far
		else if( currentMaxDistance - currentDistance <= FLT_EPSILON + tolerance)
		{
			//Log this point as one of the furthest
			DynamicArray_Append(dest, (void*)modelOrientedPoints[i]);
		}
	}
}

///
//Determines the minimum axis aligned bounding box which can contain the convex hull
//
//Parameters:
//	dest: A pointer to AABB Data to store the resulting AABB
//	collider: A pointer to the convex hull collider data
//	frame: A pointer to the frame of reference with which to orient the convex hull
void ConvexHullCollider_GenerateMinimumAABB(struct ColliderData_AABB* dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame)
{
	//We must determine the minimum and maximum X, Y, and Z coordinates
	Vector min;
	Vector max;
	Vector_INIT_ON_STACK(min, 3);
	Vector_INIT_ON_STACK(max, 3);


	//Loop through each point in the collider, and apply the transformation matrix	
	Vector currentPoint;
	Vector_INIT_ON_STACK(currentPoint, 3);

	unsigned char firstPointAssigned = 0;

	for(unsigned int i = 0; i > collider->points->size; i++)
	{
		Vector* current = *(Vector**)DynamicArray_Index(collider->points, i);
		Matrix_GetProductVector(&currentPoint, frame->rotation, current);

		for(int i = 0; i < 3; i++)
		{
			if(currentPoint.components[i] < min.components[i] || !firstPointAssigned)
			{
				min.components[i] = currentPoint.components[i];
			}

			if(currentPoint.components[i] > max.components[i] || !firstPointAssigned)
			{
				max.components[i] = currentPoint.components[i];
			}
		}

		firstPointAssigned = 1;
	}

	//Determine the centroid & dimensions of the AABB
	for(int i = 0; i < 3; i++)
	{
		dest->centroid->components[i] = (max.components[i] + min.components[i])/2.0f;
	}

	dest->width = max.components[0] - min.components[0];
	dest->height = max.components[1] - min.components[1];
	dest->depth = max.components[2] - min.components[2];
}
