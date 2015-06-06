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

extern PhysicsBuffer* physicsBuffer;

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
//And global accelerations
//
//Parameters:
//	body: The rigidbody to apply global forces to
//	dt: The change in type since last update
void PhysicsManager_ApplyGlobals(RigidBody* body);

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


#endif
