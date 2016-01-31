#ifndef CONVEXHULLCOLLIDER_H
#define CONVEXHULLCOLLIDER_H

#include "../Data/LinkedList.h"
#include "../Data/DynamicArray.h"

#include "../Math/Vector.h"
#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"

#include "ColliderEnum.h"
#include "AABBCollider.h"

//Forward declaration of Collider to avoid circular dependency
struct Collider;
//Forward declaration for AABBCollider because of include guard blocking struct decl in
//this .h file.
struct ColliderData_AABB;

///
//Defines data contained in a Face of a convex hull
struct ConvexHullCollider_Face
{
	Vector* normal;			//Face normal
	DynamicArray* indicesOnFace;	//Indices of vertices contained on this face
};

///
//Defines data contained in a convex hull collider
struct ColliderData_ConvexHull
{
	DynamicArray* points;
	DynamicArray* faces;
	DynamicArray* edges;
};


///
//Allocates memory for a new Convex Hull face
//
//REturns:
//	A pointer to a newly allocated, uninitialized, convex hull face
struct ConvexHullCollider_Face* ConvexHullCollider_AllocateFace();

///
//Initializes a ConvexHull_Face
//
//Parameters:
//	face: A Pointer to the ConvexHull_Face to initialize
void ConvexHullCollider_InitializeFace(struct ConvexHullCollider_Face* face);

///
//Initializes a deep copy of a ConvexHullFace
//This means any pointers will point to a Newly Allocated instance of identical memory unless otherwise noted
//
//Parameters:
//	copy: A pointer to an uninitialized ConvexHullFace to initialize as a deep copy
//	original: A pointer to a ConvexHullFace to deep copy
void ConvexHullCollider_InitializeFaceDeepCopy(struct ConvexHullCollider_Face* copy, struct ConvexHullCollider_Face* original);

///
//Frees memory allocated by a ConvexHull_Face
//
//Parameters:
//	face: The convex hull face to free
void ConvexHullCollider_FreeFace(struct ConvexHullCollider_Face* face);

///
//Allocates memory for a new convex hull collider data set
//
//Returns:
//	Pointer to a newly allocated convex hull collider data set
struct ColliderData_ConvexHull* ConvexHullCollider_AllocateData();

///
//Initializes a convex hull data set
//
//Parameters:
//	convexData: THe convex Hull Data Set being initialized
void ConvexHullCollider_InitializeData(struct ColliderData_ConvexHull* convexData);

///
//Initializes a convex hull collider
//
//Parameters:
//	collider: The colider being initialized
void ConvexHullCollider_Initialize(struct Collider* collider);

///
//Initializes a deep copy of a ConvexHullCollider
//This means any pointers will point to Newly Allocated instances of identical memory unless otherwise noted
//
//Parameters:
//	copy: A pointer to an uninitialized ConvexHullCollider to initialize as a deep copy
//	original: A pointer to a ConvexHullCollider to deep copy
void ConvexHullCollider_InitializeDeepCopy(struct Collider* copy, struct Collider* original);

///
//Frees a convex hull collider data set
//
//Parameters:
//	colliderData: A pointer to the convex hull collider data to free
void ConvexHullCollider_FreeData(struct ColliderData_ConvexHull* colliderData);

///
//Adds a point to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull having a point added
//	point: A pointer to a vector of dimension 3 representing the point to add
void ConvexHullCollider_AddPoint(struct ColliderData_ConvexHull* collider, const Vector* point);

///
//Adds an axis to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull collider having an axis added
//	axis: A pointer to a vector of dimension 3 representing the axis to add
//void ConvexHullCollider_AddAxis(struct ColliderData_ConvexHull* collider, const Vector* axis);

///
//Adds a face to a convex hull collider
//
//Parameters:
//	collider: A pointer to the convex hull collider having a face added
//	face: A pointer to the face being added
void ConvexHullCollider_AddFace(struct ColliderData_ConvexHull* collider, const struct ConvexHullCollider_Face* face);

///
//Adds an edge to a convex hull collider
//
//Parameters:
//	Collider: A pointer to the convex hull collider having an axis added
//	axis: A pointer to a vector of dimension 3 representing the direction vector of an edge on the collider
void ConvexHullCollider_AddEdge(struct ColliderData_ConvexHull* collider, const Vector* edgeDirection);

///
//Makes a cube convex hull collider from a blank initialized convex hull collider
//
//Parameters:
//	collider: A pointer to a convex hull collider with no current points or axis to make into a cube
//	sideLength: The initial sidelength of the cube collider to make
void ConvexHullCollider_MakeCubeCollider(struct ColliderData_ConvexHull* collider, float sideLength);

///
//Makes a rectangular convex hull collider from a blank initialized convex hull collider
//
//Parameters:
//	collider: A pointer to a convex hullcollider with no current points or axis to make into a rectangle
//	width: The width of the collider
//	Height: The height of the collider
//	depth: The depth of the collider
void ConvexHullCollider_MakeRectangularCollider(struct ColliderData_ConvexHull* collider, float width, float height, float depth);

///
//Gets the points of the collider oriented in world space to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider points
//	collider: The collider of which to orient the points of
//	frame: The frame of reference with which to orient the points
void ConvexHullCollider_GetOrientedWorldPoints(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame);

///
//Gets the points of the collider oriented in model space to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider points
//	collider: The collider of which to orient the points of
//	frame: A pointer to the frame of reference with which to orient the points
void ConvexHullCollider_GetOrientedModelPoints(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame);

///
//Gets the axes of a convex hull collider oriented to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider axes
//	collider: A pointer to the collider of which to orient the axes of
//	frame: A pointer to the frame of reference with which to orient the axes
void ConvexHullCollider_GetOrientedAxes(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame);

///
//Gets the edges of a convex hull collider oriented to match a given frame of reference
//
//Parameters:
//	dest: An array of pointers to initialized vectors of dimension 3 to store the oriented collider edges
//	collider: A pointer to the collider of which to orient the edges of
//	frame: A pointer to the frame of reference with which to orient the edges
void ConvexHullCollider_GetOrientedEdges(Vector** dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame);

///
//Determines the subset set of a convex hull colliders points oriented to a given orientation furthest in a given direction.
//These points are found in MODEL SPACE! The components of the vectors in the dynamic array POINT TO the components of the vectors in the model oriented points array!
//
//Parameters:
//	dest: A dynamic array to store the set of points in
//	collider: A pointer to a convex hull collider to get the subset of points from
//	modelOrientedPoints: An array of pointers to vectors representing the colliders points oriented into modelSpace
//	frame: The frame to orient the collider's points with
//	direction: The direction in which the desired set of points are furthest in.
void ConvexHullCollider_GetFurthestPoints(DynamicArray* dest, const struct ColliderData_ConvexHull* collider, const Vector** modelOrientedPoints, const Vector* direction);

///
//Determines the minimum axis aligned bounding box which can contain the convex hull
//
//Parameters:
//	dest: A pointer to AABB Data to store the resulting AABB
//	collider: A pointer to the convex hull collider data
//	frame: A pointer to the frame of reference with which to orient the convex hull
void ConvexHullCollider_GenerateMinimumAABB(struct ColliderData_AABB* dest, const struct ColliderData_ConvexHull* collider, const FrameOfReference* frame);

#endif
