#ifndef AABBCollider_H
#define AABBCollider_H

#include "ColliderEnum.h"
#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"
#include "ConvexHullCollider.h"

//Forward declaration of Collider to avoid circular dependency
struct Collider;
//enum ColliderType;


struct ColliderData_AABB
{
	//WARNING:
	//IF YOU CHANGE THE ORDER OF THESE, REWRITE OCTTREE_NODE_DOESOBJECTCOLLIDE!
	float width;		//X Dimension of bounding box
	float height;		//Y Dimension of bounding box
	float depth;		//Z Dimension of bounding box
	Vector* centroid;	//The centroid of the AABB
};

///
//Allocates memory for a new Axis Aligned Bounding Box collider data
//
//returns:
//	Pointer to newly allocated uninitialized axis aligned bounding box collider data
struct ColliderData_AABB* AABBCollider_AllocateData();

///
//Initializes an AABB collider's data set
//
//PArameters:
//	AABB: The axis aligned bounding box data set being initialized
//	width: The width of the AABB
//	height: The height of the AABB
//	depth: The depth of the AABB
//	centroid: A pointer to a vector to copy as the centroid of the AABB
void AABBCollider_InitializeData(struct ColliderData_AABB* AABB, const float width, const float height, const float depth, const Vector* centroid);

///
//Initializes an axis aligned bounding box collider
//
//parameters:
//	collider: The collider to initialize
//	width: The width of the bounding box
//	height: The height of the bounding box
//	depth: The depth of the bounding box
//	Centroid: A pointer to a vector to copy as the centroid of te AABB
void AABBCollider_Initialize(struct Collider* collider, float width, float height, float depth, const Vector* centroid);

///
//Initializes an axis aligned bounding box collider fit to a given mesh
//
//Parameters:
//	collider: The collider to initialize
//	mesh: The mesh to base the AABB collider's dimensions off of
void AABBCollider_InitializeFromMesh(struct Collider* collider, const Mesh* mesh);

///
//Frees an axis aligned collider data set
//
//Parameters:
//	colliderData: A pointer to the axis aligned bounding box collider data to free
void AABBCollider_FreeData(struct ColliderData_AABB* colliderData);

///
//Gets the dimensions of an AABB scaled by the dimensions of a frame of reference
//
//Parameters:
//	dest: A pointer to a AABB Collider data set to store the scaled dimensions
//	colliderData: A pointer to the un-scaled AABB Collider data set
//	FoR: A pointer to the frame of reference by which to scale the collider data set
void AABBCollider_GetScaledDimensions(struct ColliderData_AABB* dest, const struct ColliderData_AABB* colliderData, const FrameOfReference* FoR);

///
//Takes an AABB Collider and represents it as a Convex Hull Collider
//
//Parameters:
//	dest:A pointer to an initialized & allocated convex hull collider data to store the representation
//	srcCollider: A pointer to an AABB collider data set to be represented as a convex hull collider
void AABBCollider_ToConvexHullCollider(struct ColliderData_ConvexHull* dest, const struct ColliderData_AABB* srcCollider);

#endif
