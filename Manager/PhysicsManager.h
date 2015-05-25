#ifndef PHYSICSMANAGER_H
#define PHYSICSMANAGER_H

#if defined __linux__
//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#endif


#include "CollisionManager.h"

#include "../GObject/GObject.h"
#include "../Data/DynamicArray.h"
#include "../Data/LinkedList.h"

typedef struct PhysicsBuffer
{
	LinkedList* globalForces;			//Contains the list of global forces to apply to all bodies upon each update
	LinkedList* globalAccelerations;	//Contains the listof global accelerations to apply to all bodies upon each update
} PhysicsBuffer;

static PhysicsBuffer* physicsBuffer;

///
//Allocates memory for a new Physics Buffer
//
//Returns:
//	Pointer to a newly allocated physics buffer
static PhysicsBuffer* PhysicsManager_AllocateBuffer();

///
//Initializes a physics buffer
//
//Parameters:
//	buffer: The physics buffer to initialize
static void PhysicsManager_InitializeBuffer(PhysicsBuffer* buffer);

///
//Frees memory taken by a physics buffer
//
//Parameters:
//	buffer: The buffer to free the memory of
static void PhysicsManager_FreeBuffer(PhysicsBuffer* buffer);

///
//Initializes the physics manager
void PhysicsManager_Initialize();

///
//Frees internal data of the physics manager
void PhysicsManager_Free();

///
//Gets a reference to the internal physics buffer of the physics manager
//
//Returns:
//	A pointer to the physics buffer
PhysicsBuffer* PhysicsManager_GetPhysicsBuffer();

///
//Adds a global force to the list of global forces
//
//Parameters:
//	force: A pointer to a vector of dimension 3 representing the force to add
void PhysicsManager_AddGlobalForce(Vector* force);

///
//Adds a global acceleration to the list of global accelerations
//
//Parameters:
//	acceleration: A pointer to a vector of dimension 3 representing the acceleration to add
void PhysicsManager_AddGlobalAcceleration(Vector* acceleration);

///
//Updates the Physics Manager
//
//Parameters:
//	gameObjects: the physics components of the list of gameObjects
void PhysicsManager_Update(LinkedList* gameObjects);

///
//Updates the Rigidbody components of all gameObjects
//
//Parameters:
//	gameObjects: THe linked list of gameobjects to update their rigidBodies
void PhysicsManager_UpdateBodies(LinkedList* gameObjects);

///
//Applies all global forces to the given rigidbody
//
//Parameters:
//	body: The rigidbody to apply global forces to
//	dt: The change in type since last update
void PhysicsManager_ApplyGlobalForces(RigidBody* body, float dt);

///
//Updates the linear physics of a rigidbody
//This determines calculations of acceleration, velocity, and position from netForce, netImpulse, and 1/mass
//
//Parameters:
//	body: The body to update
//	dt: The change in time since last update
void PhysicsManager_UpdateLinearPhysicsOfBody(RigidBody* body, float dt);

///
//Updates the rotational physics of a rigidbody
//This determines calculations of angular acceleration, angular velocity, and orientation / rotation
//from net torque and inverse Inertia
//
//Parameters:
//	body: The body to update
//	dt: The change in time since last update
void PhysicsManager_UpdateRotationalPhysicsOfBody(RigidBody* body, float dt);

///
//Updates the Frame of reference component of all gameObjects to match their rigidbodies
//
//Parameters:
//	gameObjects: the linked list of gameObjects to update their rigidbodies
void PhysicsManager_UpdateObjects(LinkedList* gameObjects);

///
//Resolves all collisions in a linked list
//
//Parameters:
//	collisions: A linked list of all collisions detected which need resolving
void PhysicsManager_ResolveCollisions(LinkedList* collisions);

///
//Resolves a collision
//
//Parameters:
//	collisions: The collision which needs to be resolved
static void PhysicsManager_ResolveCollision(struct Collision* collision);

///
//Determines if a collision needs to be resolved, or if it is resolving itself
//
//PArameters:
//	collision: The collision to test
//
//REturns:
//	0 if no collision resolution is needed
//	1 if collision resolution is needed
static unsigned char PhysicsManager_IsResolutionNeeded(struct Collision* collision);

///
//Takes two intersecting objects and restores them to the moment their surfaces touched
//
//Parameters:
//	collision: The collision to decouple
static void PhysicsManager_DecoupleCollision(struct Collision* collision);

///
//Determines the point of contact in a decoupled collision
//In the case where at least one of the objects is an AABB, there will be two separate collision points,
//One on each object.
//
//Parameters:
//	dest: Array of two Vectors to store the points of collision in. These are the collision points of 
//		obj1 and obj2 respectively.
//	collision: The collision to determine the point of contact 
static void PhysicsManager_DetermineCollisionPoints(Vector** dest, struct Collision* collision);

///
//Determines the point of contact on a sphere collider
//
//Parameters:
//	dest: A pointer to a vector to store the collision point in.
//	sphere: A pointer to the sphere collider data involved in collision
//	sphereFrame: A pointer to the frame of reference of the sphere involved in the collision
//	relativeMTV: A pointer to a vector representing the MTV !pointing towards the other object!
static void PhysicsManager_DetermineCollisionPointSphere(Vector* dest, const struct ColliderData_Sphere* sphere, const FrameOfReference* sphereFrame, const Vector* relativeMTV);

///
//Determines the point of contact on an AABB Collider
//For an AABB the point of contact will always be the center of mass to prevent rotation of the object
//
//Parameters:
//	dest: A pointer to a vector to store the collision point in
//	AABBFrame: The frame of reference of the AABB involved in the collision
static void PhysicsManager_DetermineCollisionPointAABB(Vector* dest, const FrameOfReference* AABBFrame);

///
//Determines the point of contact on a Convex Hull Collider
//
//PArameters:
//	dest: A pointer to a vector to store the collision point in
//	convexHull1: A pointer to the convex hull collider data involved in the collision which we are determining the point of collision on.
//	convexFrame1: A pointer to the frame of reference of the convex hull involved in the collision which we are determining the point of collision on.
//	convexHull2: A pointer to the other convex hull collider data involved in the collision, whether the object itself has a convex hull or not.
//	convexFrame2: A pointer to the frame of reference of the other convex hull involved in the collision, whether the object itself has a convex hull or not
//	relativeMTV: A pointer to a vector representing the MTV !Pointing towards the OTHER object (convexHull2)!
static void PhysicsManager_DetermineCollisionPointConvexHull(Vector* dest,
															const struct ColliderData_ConvexHull* convexHull1, const FrameOfReference* convexFrame1,
															const struct ColliderData_ConvexHull* convexHull2, const FrameOfReference* convexFrame2,
															const Vector* relativeMTV);

///
//Determines the point of contact of a Convex Hull - Convex Hull collision when
//dealing with a Vertex on Vertex / Edge / Face collision case. That is when
//One convex hull has only a single vertex furthest in the direction of a minimum translation vector
//
//Parameters:
//	dest: A pointer to the vector to store the collision point in
//	furthestVertex: A pointer to the vector which registered as furthest in the direction of the MTV
//	frame: A pointer to the frame of reference of the convex hull which the furthestVertex belongs to
static void PhysicsManager_DetermineCollisionPointConvexHullVertex(Vector* dest, const Vector* furthestVertex, const FrameOfReference* frame);

///
//Determines the point of contact of a convex hull - convex hull collision when
//dealing with an Edge - Edge collision case. That is when both convex hulls have 
//exactly 2 vertices furthest in the direction of their respective MTVs
//
//Parameters:
//	dest: A pointer to the vector to store the collision point in
//	furthestOnHull1: A pointer to a dynamic array of vectors containing two vertices oriented in modelSpace on convexHull1 furthest in the direction of the respective relative MTV
//	convexFrame1: A pointer to the frame of reference of the convex hull which the vertices furthestOnHull1 belong to
//	furthestOnHull2: A pointer to a dynamic array of vectors containing two vertices oriented in ModelSpace on convexHull2 furthest in the direction of the respective relative MTV
//	convexFrame2: A pointer to the frame of reference of the convex Hull which the vertices furthestOnHull2 belong to
static void PhysicsManager_DetermineCollisionPointConvexHullEdge(Vector* dest,
															  const DynamicArray* furthestOnHull1, const FrameOfReference* convexFrame1,
															  const DynamicArray* furthestOnHull2, const FrameOfReference* convexFrame2);

///
//Determines the point of contact of a convex hull - convex hull collision when
//dealing with an Edge / Face - Face collision case. This is when both convex hulls have
// > 1 vertex furthest in the direction of their respective MTVs AND one convex hull has > 2
//vertices furthest in the direction of it's respective MTV
//
//Parameters:
//	dest: A pointer to he vector to store the collision in
//	furthestOnHull1: A pointer to the dynamic array of vectors containing the vertices IN WORLDSPACE belonging to convexHull1 furthest in the direction of it's respective MTV
//	convexFrame1: A pointer to the frame of reference of the convex hull which the vertices from furthestOnHull1 belong
//	furthestOnHull2: A pointer to the dynamic array of vectors containing the vertices IN WORLDSPACE belongong to convexHull2 furthest in the direction of it's respective MTV
//	convexFrame2: A pointer to the frame of reference of the convex hull whch the vertices from furthestOnHull2 belong
static void PhysicsManager_DetermineCollisionPointConvexHullFace(Vector* dest,
															  const DynamicArray* furthestOnHull1, const FrameOfReference* convexFrame1,
															  const DynamicArray* furthestOnHull2, const FrameOfReference* convexFrame2);

///
//Calculates and imparts the resulting collision impulse from the collision
//
//Parameters:
//	collision: The collision having it's resulting impulses calculated and applied
//	pointsOfCollision: Array of 2 Vectors representing the point of collision in global space for obj1 and obj2 respectively
static void PhysicsManager_ApplyCollisionImpulses(struct Collision* collision, const Vector** pointsOfCollision);

///
//Calculates and applies the linear frictional forces when two objects slide against each other
//
//Parameters:
//	collision: The collision to apply linear frictional forces to
//	pointsOfCollision: An array of 2 pointers to vectors in 3 space representing the point of collision in global space for obj1 and obj2 respectively
//	staticCoefficient: The static coefficient of friction between the two colliding surfaces
//	dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
static void PhysicsManager_ApplyLinearFrictionalImpulses(struct Collision* collision, const Vector** pointsOfCollision, const float staticCoefficient, const float dynamicCoefficient);

///
//Calculates and applies the angular frictional forces when two objects spin against each other
//
//Parameters:
//	collision: The collision to apply frictional torques to
//	staticCoefficient: The static coefficient of friction between the two colliding surfaces
//	dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
static void PhysicsManager_ApplyFrictionalTorques(struct Collision* collision, const float staticCoefficient, const float dynamicCoefficient);

#endif
