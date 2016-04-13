#ifndef AABBCollider_H
#define AABBCollider_H

#include "ColliderEnum.h"
#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"
#include "ConvexHullCollider.h"

//Forward declaration of Collider to avoid circular dependency
struct Collider;
//Forward declaration of ColliderData_ConvexHull because of include guard blocking declaration in this file
struct ColliderData_ConvexHull;

struct ColliderData_AABB_OLD
{
	//WARNING:
	//IF YOU CHANGE THE ORDER OF THESE, REWRITE OCTTREE_NODE_DOESOBJECTCOLLIDE!
	float width;		//X Dimension of bounding box
	float height;		//Y Dimension of bounding box
	float depth;		//Z Dimension of bounding box
	Vector* centroid;	//The centroid of the AABB
};

struct ColliderData_AABB
{
	float min[3];
	float max[3];
};

///
//Allocates memory for a new Axis Aligned Bounding Box collider data
//
//returns:
//	Pointer to newly allocated uninitialized axis aligned bounding box collider data
int AABBCollider_AllocateData();

///
//Initializes an AABB collider's data set
//
//Parameters:
//	AABB: The axis aligned bounding box data set being initialized
//	min: The minimum xyz point on the AABB in object space
//	max: The maximum xyz point on the AABB in object space
void AABBCollider_InitializeData(struct ColliderData_AABB* AABB, const float min[3], const float max[3]);

///
//Initializes an axis aligned bounding box collider
//
//parameters:
//	collider: The collider to initialize
//	min: The minimum XYZ point on the AABB in object space
//	max: The maximum XYZ point on the AABB in object space
void AABBCollider_Initialize(struct Collider* collider, const float min[3], const float max[3]);

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
//	colliderDataID: The ID of the memoryunit containing the AABB collider data
void AABBCollider_FreeData(int colliderDataID);

///
//Gets the dimensions of an AABB scaled by the dimensions of a frame of reference
//
//Parameters:
//	dest: A pointer to a AABB Collider data set to store the scaled dimensions
//	colliderData: A pointer to the un-scaled AABB Collider data set
//	FoR: A pointer to the frame of reference by which to scale the collider data set
void AABBCollider_GetScaledDimensions(struct ColliderData_AABB* dest, const struct ColliderData_AABB* colliderData, const FrameOfReference* FoR);

///
//Gets an AABB oriented in world space
//
//Parameters:
//	dest: A pointer to the destination to store the AABB in world space
//	colliderData: A pointer to the AABB toorient in worldspace
//	frame: A pointer to the frame of reference with which to orient the AABB
void AABBCollider_GetWorldAABB(struct ColliderData_AABB* dest, const struct ColliderData_AABB* colliderData, const FrameOfReference* frame);

///
//Takes an AABB Collider and represents it as a Convex Hull Collider
//
//Parameters:
//	dest:A pointer to an initialized & allocated convex hull collider data to store the representation
//	srcCollider: A pointer to an AABB collider data set to be represented as a convex hull collider
void AABBCollider_ToConvexHullCollider(struct ColliderData_ConvexHull* dest, const struct ColliderData_AABB* srcCollider);

///
//Updates the worldspace data of this collider to match that of the given frame of reference
//
//Parameters:
//	colliderDataID: The ID of the collider
//	frame: A pointer to the frame of reference with which to orient this collider
void AABBCollider_Update(const unsigned int colliderDataID, FrameOfReference* frame);

#endif
