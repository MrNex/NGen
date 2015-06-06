#include "ConvexHullCollider.h"

#include <stdlib.h>

#include "Collider.h"

#include "../Manager/AssetManager.h"

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
	convexHullData->points = LinkedList_Allocate();
	LinkedList_Initialize(convexHullData->points);

	convexHullData->axes = LinkedList_Allocate();
	LinkedList_Initialize(convexHullData->axes);

	convexHullData->edges = LinkedList_Allocate();
	LinkedList_Initialize(convexHullData->edges);
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
	//Free all vectors in points & axes linked lists
	struct LinkedList_Node* current = colliderData->points->head;
	struct LinkedList_Node* next = NULL;

	while(current != NULL)
	{
		next = current->next;

		Vector* point = (Vector*)current->data;
		Vector_Free(point);

		current = next;
	}
	LinkedList_Free(colliderData->points);

	current = colliderData->axes->head;
	next = NULL;

	while(current != NULL)
	{
		next = current->next;

		Vector* axis = (Vector*)current->data;
		Vector_Free(axis);

		current = next;
	}
	LinkedList_Free(colliderData->axes);

	current = colliderData->edges->head;
	next = NULL;

	while(current != NULL)
	{
		next = current->next;

		Vector* axis = (Vector*)current->data;
		Vector_Free(axis);

		current = next;
	}
	LinkedList_Free(colliderData->edges);

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
	//The linked list will not manipulate the data
	LinkedList_Append(collider->points, (Vector*)point);
}

///
//Adds an axis to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull collider having an axis added
//	axis: A pointer to a vector of dimension 3 representing the axis to add
void ConvexHullCollider_AddAxis(struct ColliderData_ConvexHull* collider, const Vector* axis)
{
	//The linked list will not manipulate the data
	LinkedList_Append(collider->axes, (Vector*)axis);
}

///
//Adds an edge to a convex hull collider
//
//Parameters:
//	Collider: A pointer to the convex hull collider having an axis added
//	axis: A pointer to a vector of dimension 3 representing the direction vector of an edge on the collider
void ConvexHullCollider_AddEdge(struct ColliderData_ConvexHull* collider, const Vector* edgeDirection)
{
	//The linked list will never manipulate the data
	LinkedList_Append(collider->edges, (Vector*)edgeDirection);
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
	//1) Lower Right Front Corner
	Vector* point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//2) Lower Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//3) Lower Left Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//4) Lower Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = -sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//5) Upper Right Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//6) Upper Right Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//7) Upper LEft Back Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = -sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//8) Upper Left Front Corner
	point = Vector_Allocate();
	Vector_Initialize(point, 3);

	point->components[0] = -sideLength / 2.0f;
	point->components[1] = sideLength / 2.0f;
	point->components[2] = sideLength / 2.0f;

	ConvexHullCollider_AddPoint(collider, point);

	//Second add axes & edges (same things)
	//Right/Left face
	Vector* axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	Vector* edge = Vector_Allocate();\
		Vector_Initialize(edge, 3);


	axis->components[0] = 1.0f;
	Vector_Copy(edge, axis);

	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);


	//Top/Bottom face
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);


	axis->components[1] = 1.0f;
	Vector_Copy(edge, axis);


	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);


	//Front/Back face
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	axis->components[2] = 1.0f;
	Vector_Copy(edge, axis);


	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);

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

	//Second add axes & edges (same things)
	//Right/Left face
	Vector* axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	Vector* edge = Vector_Allocate();\
		Vector_Initialize(edge, 3);


	axis->components[0] = 1.0f;
	Vector_Copy(edge, axis);

	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);


	//Top/Bottom face
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);


	axis->components[1] = 1.0f;
	Vector_Copy(edge, axis);


	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);


	//Front/Back face
	axis = Vector_Allocate();
	Vector_Initialize(axis, 3);
	edge = Vector_Allocate();
	Vector_Initialize(edge, 3);

	axis->components[2] = 1.0f;
	Vector_Copy(edge, axis);


	ConvexHullCollider_AddAxis(collider, axis);
	ConvexHullCollider_AddEdge(collider, edge);
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
	for(int i = 0; i < collider->points->size; i++)
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
	struct LinkedList_Node* currentPoint = collider->points->head;
	for(unsigned int i = 0; i < collider->points->size; i++)
	{
		//Rotate and scale each point
		Matrix_GetProductVector(dest[i], &trans, (Vector*)currentPoint->data);

		//Move to next point
		currentPoint = currentPoint->next;
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
	struct LinkedList_Node* currentAxis = collider->axes->head;
	for(unsigned int i = 0; i < collider->axes->size; i++)
	{
		//Rotate each axis 
		Matrix_GetProductVector(dest[i], frame->rotation, (Vector*)currentAxis->data);

		//Move to next axis
		currentAxis = currentAxis->next;
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
	struct LinkedList_Node* currentEdge = collider->edges->head;
	for(unsigned int i = 0; i < collider->edges->size; i++)
	{
		//Rotate each edge 
		Matrix_GetProductVector(dest[i], frame->rotation, (Vector*)currentEdge->data);

		//Move to next edge
		currentEdge = currentEdge->next;
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
		else if(currentDistance == currentMaxDistance)
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
	//Generate a transformation matrix to rotate/scale each point of the collider
	//Scratch that, scaling will happen at a later stage as if this were an AABB which needed to be scaled.
	//Matrix trans;
	//Matrix_INIT_ON_STACK(trans, 3, 3);
	//Matrix_GetProductMatrix(&trans, frame->rotation, frame->scale);

	//We must determine the minimum and maximum X, Y, and Z coordinates
	Vector min;
	Vector max;
	Vector_INIT_ON_STACK(min, 3);
	Vector_INIT_ON_STACK(max, 3);


	//Loop through each point in the collider, and apply the transformation matrix	
	struct LinkedList_Node* current = collider->points->head;
	Vector currentPoint;
	Vector_INIT_ON_STACK(currentPoint, 3);

	unsigned char firstPointAssigned = 0;

	while(current != NULL)
	{
		Matrix_GetProductVector(&currentPoint, frame->rotation, (Vector*)current->data);

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

		current = current->next;
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
