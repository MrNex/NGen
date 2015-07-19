#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "../Data/DynamicArray.h"
#include "../Render/FrameOfReference.h"

typedef struct RigidBody
{
	float coefficientOfRestitution;		//How elastic will this float act in a collision (0.0f - 1.0f)
	float staticFriction;			//Static coefficient of friction for the surface attached to this object
	float dynamicFriction;			//Dynamic coefficient of friction for the surface attached to this object
	float rollingResistance;		//Coefficient of rolling Resistance
	float inverseMass;			//Because schwartz
	Matrix* inverseInertia;			//inverse moment of inertia matrix
	Matrix* inertia;			//Moment of inertia matrix
	Vector* netForce;			//Total net force this instant
	Vector* previousNetForce;		//Total net force previous instant
	Vector* netImpulse;			//net impulses this instant
	Vector* netTorque;			//Total net torque this instant
	Vector* previousNetTorque;		//net Torque previous instant
	Vector* netInstantaneousTorque;		//Total instantaneous torque this instant
	Vector* acceleration;			//current acceleration of point mass
	Vector* angularAcceleration;		//current angular acceleration
	Vector* velocity;			//current velocity of point mass
	Vector* angularVelocity;		//Current angular velocity
	FrameOfReference* frame;		//Position and orientation of point mass in global space
	unsigned char freezeTranslation;	//Freezes the rigidbody so it can not have any linear forces applied
	unsigned char freezeRotation;		//Freezes the rigidbody so it cannot have any torques applied
	unsigned char physicsOn;		//Boolean to turn physics off. 1 = on | 0 = off.
} RigidBody;

///
//Allocates memory for a rigidBody
//
//Returns:
//	Pointer to a newly allocated RigidBody in memory
RigidBody* RigidBody_Allocate(void);

///
//Initializes a RigidBody
//
//PArameters:
//	body: THe rigid body to initialize
//	startingFrame: The frame the rigidbody should use to initialize itself in world space
//	mass: The mass of the rigidbody
void RigidBody_Initialize(RigidBody* body, const FrameOfReference* startingFrame, const float mass);

///
//Frees resources allocated by a rigidbody
//
//Parameters:
//	body: The rigidbody to free
void RigidBody_Free(RigidBody* body);

///
//Uses a rigid bodies frame of reference to determine the rectangular prisms Width Depth and Height,
//Then uses them to calculate the moment of inertia tensor.
//This function assumes that before scaling the cube has a space of -1 to 1 in all dimensions.
//
//Parameters:
//	body: The rigid body to calculate and set the inertia tensor of
void RigidBody_SetInertiaOfCuboid(RigidBody* body);

///
//Applies a force to a rigid body
//
//Parameter:
//	body: The rigid body to apply a force to
//	forceApplied: The force being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyForce(RigidBody* body, const Vector* forceApplied, const Vector* radius);

///
//Applied an impuse to a rigid body
//
//Parameters:
//	body: The rigid body to apply an impulse to
//	impulseApplied: The impulse being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyImpulse(RigidBody* body, const Vector* impulseApplied, const Vector* radius);

///
//Applies a torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The torque to apply
void RigidBody_ApplyTorque(RigidBody* body, const Vector* torqueApplied);

///
//Applies an instantaneous torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The instantaneous torque to apply
void RigidBody_ApplyInstantaneousTorque(RigidBody* body, const Vector* torqueApplied);

///
//Calculates the linear velocity due to the angular velocity of a point on/in a rigidbody.
//This is the instantaneous linear velocity of the point around the position of an object.
//
//Parameters:
//	dest: A pointer to a vector to store the linear velocity of the given point around the given body
//	body: A pointer to the rigidbody the given point is on/in
//	point: A pointer to a vector containing the point to calculate the local linear velocity of
void RigidBody_CalculateLocalLinearVelocity(Vector* dest, const RigidBody* body, const Vector* point);

///
//Calculates the maximum linear velocity due to the angular velocity of a set of points on/in a rigidbody in a specific direction.
//This is the max instantaneous linear velocity of the set of points around the position of the body which is in a certain direction.
//Dest will contain the zero vector in the case that none of the points are travelling in the given direction
//
//Parameters:
//	dest: A pointer to the vector to store the maximum linear velocity of the given point set around the given body in the given direction
//	body: A pointer to the rigidbody the given point set is on/in
//	points: A pointer to a dynamic array containing the given point set
//	direction: A pointer to a vector which contains the desired direction of maximum velocity
void RigidBody_CalculateMaxLocalLinearVelocity(Vector* dest, const RigidBody* body, const DynamicArray* points, const Vector* direction);

///
//Calculates the moment of inertia of a rigidbody in worldspace based off of the rigidbody's orientation
//
//Parameters:
//	dest: A pointer to a matrix to store the moment of inertia traslated into worldspace
//	body: The body to find the transformed moment of inertia of
void RigidBody_CalculateMomentOfInertiaInWorldSpace(Matrix* dest, const RigidBody* body);

///
//Calculates the linear momentum of a rigidbody
//
//PArameters:
//	dest: A pointer to the vector to store the linear momentum in
//	body: A pointer to the rigidbody to calculate the linear momentum of
void RigidBody_CalculateLinearMomentum(Vector* dest, const RigidBody* body);

///
//Calculates the angular momentum of a rigidbody
//
//PArameters:
//	dest: A pointer to the vector to store the angular momentum
//	body: A pointer to the rigidbody to calculate the angular momentum of
void RigidBody_CalculateAngularMomentum(Vector* dest, const RigidBody* body);

///
//Calculates the linear momentum of a point of a rigidbody due to both linear and angular motion of the body
//
//Parameters:
//	dest: A pointer to a vector to store the momentum in
//	body: A pointer to the rigidbody to canculate the momentum of
//	radius: A pointer to a vector which contans the vector from the center of mass of the body to the point
//			at which the momentum is being calculated
void RigidBody_CalculateLinearMomentumAtPoint(Vector* dest, const RigidBody* body, const Vector* radius);

///
//Translates a rigidbody in world space
//
//Parameters:
//	body: The RigidBody to translate
//	translation: The Vector to translate by
void RigidBody_Translate(RigidBody* body, Vector* translation);

///
//Rotates a RigidBody around a specified axis by a specified amount
//
//Parameters:
//	body: The RigidBody being rotated
//	axis: The Vector to rotate the body around
//	radians: The number of radians to rotate by
void RigidBody_Rotate(RigidBody* body, const Vector* axis, float radians);

///
//Scales a RigidBody on each elementary axis
//And re-computes the moment of inertia tensor
//
//Parameters:
//	body: The rigidBody being scaled
//	scaleVector: A 3 DIM Vector comtaining the X, Y, and Z scale factors
void RigidBody_Scale(RigidBody* body, Vector* scaleVector);

///
//Sets the position of a rigidBody
//
//Parameters:
//  body: The rigidBody to set the position of
//  position: The position to move the body to
void RigidBody_SetPosition(RigidBody* body, Vector* position);

///
//Sets the rotation of a rigidBody
//
//Parameters:
//  body: The rigidbody to set the rotation of
//  position: The rotation to set the body to
void RigidBody_SetRotation(RigidBody* body, Matrix* rotation);

#endif
