#include "RigidBody.h"

#include <math.h>
#include <float.h>

///
//Allocates memory for a rigidBody
//
//Returns:
//	Pointer to a newly allocated RigidBody in memory
RigidBody* RigidBody_Allocate(void)
{
	RigidBody* body = (RigidBody*)malloc(sizeof(RigidBody));
	return body;
}

///
//Initializes a RigidBody
//
//Parameters:
//	body: THe rigid body to initialize
//	startingFrame: The frame the rigidbody should use to initialize itself in world space
//	mass: The mass of the rigidbody
void RigidBody_Initialize(RigidBody* body, const FrameOfReference* startingFrame, const float mass)
{
	body->coefficientOfRestitution = 1.0f;
	body->staticFriction = 1.0f;
	body->dynamicFriction = 1.0f;
	body->rollingResistance = 1.0f;

	if(mass != 0.0f)
	{
		body->inverseMass = 1.0f / mass;
		//Set physics to on
		body->physicsOn = 1;
	}
	else
	{
		body->physicsOn = 0;
		body->inverseMass = 0.0f;
	}

	body->inverseInertia = Matrix_Allocate();
	Matrix_Initialize(body->inverseInertia, 3, 3);

	body->inertia = Matrix_Allocate();
	Matrix_Initialize(body->inertia, 3, 3);

	body->netForce = Vector_Allocate();
	Vector_Initialize(body->netForce, 3);

	body->previousNetForce = Vector_Allocate();
	Vector_Initialize(body->previousNetForce, 3);

	body->netImpulse = Vector_Allocate();
	Vector_Initialize(body->netImpulse, 3);

	body->netTorque = Vector_Allocate();
	Vector_Initialize(body->netTorque, 3);

	body->previousNetTorque = Vector_Allocate();
	Vector_Initialize(body->previousNetTorque, 3);

	body->netInstantaneousTorque = Vector_Allocate();
	Vector_Initialize(body->netInstantaneousTorque, 3);

	body->acceleration = Vector_Allocate();
	Vector_Initialize(body->acceleration, 3);

	body->angularAcceleration = Vector_Allocate();
	Vector_Initialize(body->angularAcceleration, 3);

	body->velocity = Vector_Allocate();
	Vector_Initialize(body->velocity, 3);

	body->angularVelocity = Vector_Allocate();
	Vector_Initialize(body->angularVelocity, 3);

	body->frame = FrameOfReference_Allocate();
	FrameOfReference_InitializeDeepCopy(body->frame, startingFrame);

	//Vector_Copy(body->frame->position, startingFrame->position);
	//Matrix_Copy(body->frame->rotation, startingFrame->rotation);
	//Matrix_Copy(body->frame->scale, startingFrame->scale);

	//Set the moment of inertia
	RigidBody_SetInertiaOfCuboid(body);

	//No constraints by default
	body->freezeTranslation = 0;
	body->freezeRotation = 0;

	
}

///
//Initializes a deep copy ofn a RigidBody
//This means any pointers will point to a NEWLY ALLOCATED instance of identical memory unless otherwise noted
//
//Parameters:
//	copy: A pointer to an uninitialized rigidbody to initialize as a deep copy
//	original: A pointer to a rigidbody to deep copy
void RigidBody_InitializeDeepCopy(RigidBody* copy, RigidBody* original)
{
	RigidBody_Initialize(copy, original->frame, 0.0f);

	copy->coefficientOfRestitution = original->coefficientOfRestitution;
	copy->staticFriction = original->staticFriction;
	copy->dynamicFriction = original->dynamicFriction;
	copy->rollingResistance = original->rollingResistance;
	copy->inverseMass = original->inverseMass;

	Matrix_Copy(copy->inverseInertia, original->inverseInertia);
	Matrix_Copy(copy->inertia, original->inertia);

	Vector_Copy(copy->netForce, original->netForce);
	Vector_Copy(copy->previousNetForce, original->previousNetForce);
	Vector_Copy(copy->netImpulse, original->netImpulse);
	Vector_Copy(copy->netTorque, original->netTorque);
	Vector_Copy(copy->previousNetTorque, original->previousNetTorque);
	Vector_Copy(copy->netInstantaneousTorque, original->netInstantaneousTorque);
	Vector_Copy(copy->acceleration, original->acceleration);
	Vector_Copy(copy->angularAcceleration, original->angularAcceleration);
	Vector_Copy(copy->velocity, original->velocity);
	Vector_Copy(copy->angularVelocity, original->angularVelocity);

	copy->freezeRotation = original->freezeRotation;
	copy->freezeTranslation = original->freezeTranslation;
	copy->physicsOn = original->physicsOn;
}

///
//Frees resources allocated by a rigidbody
//
//Parameters:
//	body: The rigidbody to free
void RigidBody_Free(RigidBody* body)
{
	Matrix_Free(body->inverseInertia);
	Matrix_Free(body->inertia);
	Vector_Free(body->netForce);
	Vector_Free(body->previousNetForce);
	Vector_Free(body->netImpulse);
	Vector_Free(body->netTorque);
	Vector_Free(body->previousNetTorque);
	Vector_Free(body->netInstantaneousTorque);
	Vector_Free(body->acceleration);
	Vector_Free(body->angularAcceleration);
	Vector_Free(body->velocity);
	Vector_Free(body->angularVelocity);
	FrameOfReference_Free(body->frame);
	free(body);
}


///
//CHecks if a rigidbody has translational freedom
//
//Parameters:
//	body: The rigidbody to check
//
//Returns:
//	1 if the rigidbody has translational freedom, else 0
unsigned char RigidBody_CanTranslate(RigidBody* body)
{
	return body != NULL && body->freezeTranslation != 1 && body->inverseMass > FLT_EPSILON;
}

///
//CHecks if a rigidbody has rotational freedom
//
//Parameters:
//	body: The rigidbody to check
//
//Returns:
//	1 if the rigidbody has rotational freedom, else 0
unsigned char RigidBody_CanRotate(RigidBody* body)
{
	return body != NULL && body->freezeRotation != 1 && body->inverseMass > FLT_EPSILON;
}

///
//Uses a rigid bodies frame of reference to determine the rectangular prisms Width Depth and Height,
//Then uses them to calculate the moment of inertia tensor.
//This function assumes that before scaling the cube has a space of -1 to 1 in all dimensions.
//
//Parameters:
//	body: The rigid body to calculate and set the inertia tensor of
void RigidBody_SetInertiaOfCuboid(RigidBody* body)
{
	float width = 2.0f * Matrix_GetIndex(body->frame->scale, 0, 0);	//Width of cuboid
	float height = 2.0f * Matrix_GetIndex(body->frame->scale, 1, 1);//Height of cuboid
	float depth = 2.0f * Matrix_GetIndex(body->frame->scale, 2, 2);	//depth of cuboid

	float IX = (1.0f / 12.0f) * (powf(height, 2.0f) + powf(depth, 2.0f));//Inertia / mass on X axis
	float IY = (1.0f / 12.0f) * (powf(width, 2.0f) + powf(depth, 2.0f)); //Inertia / mass on Y axis
	float IZ = (1.0f / 12.0f) * (powf(width, 2.0f) + powf(height, 2.0f));//Inertia / mass on Z axis

	float iIX = (1.0f / IX) * body->inverseMass;	//Inverse moment of inertia on X axis
	float iIY = (1.0f / IY) * body->inverseMass;	//Inverse moment of inertia on Y axis
	float iIZ = (1.0f / IZ) * body->inverseMass;	//Inverse moment of inertia on Z axis

	//Assign values ot matrix
	//Inverse
	*Matrix_Index(body->inverseInertia, 0, 0) = iIX;
	*Matrix_Index(body->inverseInertia, 1, 1) = iIY;
	*Matrix_Index(body->inverseInertia, 2, 2) = iIZ;
	//Inertia
	*Matrix_Index(body->inertia, 0, 0) = IX/body->inverseMass;
	*Matrix_Index(body->inertia, 1, 1) = IY/body->inverseMass;
	*Matrix_Index(body->inertia, 2, 2) = IZ/body->inverseMass;

}

///
//Applies a force to a rigid body
//
//Parameter:
//	body: The rigid body to apply a force to
//	forceApplied: The force being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyForce(RigidBody* body, const Vector* forceApplied, const Vector* radius)
{
	//If the body is not linearly frozen
	if(!body->freezeTranslation)
	{
		Vector_Increment(body->netForce, forceApplied);
	}

	//If the body's rotation is not frozen
	if(!body->freezeRotation)
	{
		Vector torque;
		Vector_INIT_ON_STACK(torque, 3);

		Vector_CrossProduct(&torque, radius, forceApplied);
		Vector_Increment(body->netTorque, &torque);
	}
}

///
//Applied an impuse to a rigid body
//
//Parameters:
//	body: The rigid body to apply an impulse to
//	impulseApplied: The impulse being applied
//	radius: The vector from the Center of Mass to the contact point on surface where force is applied
//		For purposes of preventing rotation make the radius 0.
void RigidBody_ApplyImpulse(RigidBody* body, const Vector* impulseApplied, const Vector* radius)
{
	//If the body is not linearly frozen
	if(!body->freezeTranslation)
	{
		Vector_Increment(body->netImpulse, impulseApplied);
	}

	//If the body's rotation is not frozen
	if(!body->freezeRotation)
	{
		Vector instantTorque;
		Vector_INIT_ON_STACK(instantTorque, 3);

		Vector_CrossProduct(&instantTorque, radius, impulseApplied);
		Vector_Increment(body->netInstantaneousTorque, &instantTorque);
	}
}

///
//Applies a torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The torque to apply
void RigidBody_ApplyTorque(RigidBody* body, const Vector* torqueApplied)
{
	//If the body's rotation is not frozen
	if(!body->freezeRotation)
	{
		Vector_Increment(body->netTorque, torqueApplied);
	}
}

///
//Applies an instantaneous torque to a rigidbody
//
//Parameters:
//	body: The body to apply a torque to
//	torqueApplied: The instantaneous torque to apply
void RigidBody_ApplyInstantaneousTorque(RigidBody* body, const Vector* instantaneousTorqueApplied)
{
	//If the body's rotation is not frozen
	if(!body->freezeRotation)
	{
		Vector_Increment(body->netInstantaneousTorque, instantaneousTorqueApplied);
	}
}

///
//Calculates the linear velocity due to the angular velocity of a point on/in a rigidbody.
//This is the instantaneous linear velocity of the point around the position of an object.
//
//Parameters:
//	dest: A pointer to a vector to store the linear velocity of the given point around the given body
//	body: A pointer to the rigidbody the given point is on/in
//	point: A pointer to a vector containing the point to calculate the local linear velocity of
void RigidBody_CalculateLocalLinearVelocity(Vector* dest, const RigidBody* body, const Vector* point)
{
	Vector_CrossProduct(dest, body->angularVelocity, point);
}

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
void RigidBody_CalculateMaxLocalLinearVelocity(Vector* dest, const RigidBody* body, const DynamicArray* points, const Vector* direction)
{
	float maxDotProduct = 0.0f;
	float currentDotProduct = 0.0f;

	Vector* currentPoint;

	Vector linearVelocityAtPoint;
	Vector_INIT_ON_STACK(linearVelocityAtPoint, 3);


	//For each of the furthest points, determine the one which has the highest linear velocity due to angular velocity in the direction of the relative MTV
	for(unsigned int i = 0; i < points->size; i++)
	{
		//Get the current point and calculate it's current local linear velocity
		currentPoint = (Vector*)DynamicArray_Index((DynamicArray*)points, i);
		RigidBody_CalculateLocalLinearVelocity(&linearVelocityAtPoint, body, currentPoint);

		//Determine if it is greater in the direction given than our current greatest
		currentDotProduct = Vector_DotProduct(&linearVelocityAtPoint, direction);
		if(currentDotProduct > maxDotProduct)
		{
			maxDotProduct = currentDotProduct;
			Vector_Copy(dest, &linearVelocityAtPoint);
		}
	}

	if(maxDotProduct == 0.0f)
	{
		Vector_Copy(dest, &Vector_ZERO);
	}
}

///
//Calculates the moment of inertia of a rigidbody in worldspace based off of the rigidbody's orientation
//
//Parameters:
//	dest: A pointer to a matrix to store the moment of inertia traslated into worldspace
//	body: The body to find the transformed moment of inertia of
void RigidBody_CalculateMomentOfInertiaInWorldSpace(Matrix* dest, const RigidBody* body)
{
	//I' = TIT^-1
	//And the transpose of an orthogonal matrix is it's inverse
	Matrix iRotation;		//Inverse of rotation matrix
	Matrix_INIT_ON_STACK(iRotation, 3, 3);

	Matrix_GetTranspose(&iRotation, body->frame->rotation);
	Matrix_GetProductMatrix(dest, body->inertia, &iRotation);
	Matrix_TransformMatrix(body->frame->rotation, dest);
}

///
//Calculates the linear momentum of a rigidbody
//
//PArameters:
//	dest: A pointer to the vector to store the linear momentum in
//	body: A pointer to the rigidbody to calculate the linear momentum of
void RigidBody_CalculateLinearMomentum(Vector* dest, const RigidBody* body)
{
	//Let the linear momentum of the body be denoted as 'j'
	//j = mv
	float mass = body->inverseMass != 0.0f ? 1.0f / body->inverseMass : 0.0f;
	Vector_GetScalarProduct(dest, body->velocity, mass);
}

///
//Calculates the angular momentum of a rigidbody
//
//PArameters:
//	dest: A pointer to the vector to store the angular momentum
//	body: A pointer to the rigidbody to calculate the angular momentum of
void RigidBody_CalculateAngularMomentum(Vector* dest, const RigidBody* body)
{
	//Let 'l' denote the angular momentum and 'w' denote the angular velocity
	//Let "I'" denote the moment of intertia in worldspace
	//l = I'w
	Matrix worldInertia;
	Matrix_INIT_ON_STACK(worldInertia, 3, 3);

	RigidBody_CalculateMomentOfInertiaInWorldSpace(&worldInertia, body);

	Matrix_GetProductVector(dest, &worldInertia, body->angularVelocity);
}

///
//Calculates the linear momentum of a point of a rigidbody due to both linear and angular motion of the body
//
//Parameters:
//	dest: A pointer to a vector to store the momentum in
//	body: A pointer to the rigidbody to canculate the momentum of
//	radius: A pointer to a vector which contans the vector from the center of mass of the body to the point
//			at which the momentum is being calculated
void RigidBody_CalculateLinearMomentumAtPoint(Vector* dest, const RigidBody* body, const Vector* radius)
{
	Vector velocityAtPoint;
	Vector_INIT_ON_STACK(velocityAtPoint, 3);

	RigidBody_CalculateLocalLinearVelocity(&velocityAtPoint, body, radius);

	Vector_Increment(&velocityAtPoint, body->velocity);

	float mass = body->inverseMass != 0.0f ? 1.0f / body->inverseMass : 0.0f;
	Vector_GetScalarProduct(dest, &velocityAtPoint, mass);
}

///
//Translates a rigidbody in world space
//
//Parameters:
//	body: The RigidBody to translate
//	translation: The Vector to translate by
void RigidBody_Translate(RigidBody* body, Vector* translation)
{
	FrameOfReference_Translate(body->frame, translation);
}

///
//Rotates a RigidBody around a specified axis by a specified amount
//
//Parameters:
//	body: The RigidBody being rotated
//	axis: The Vector to rotate the body around
//	radians: The number of radians to rotate by
void RigidBody_Rotate(RigidBody* body, const Vector* axis, float radians)
{
	FrameOfReference_Rotate(body->frame, axis, radians);
}

///
//Scales a RigidBody on each elementary axis
//And re-computes the moment of inertia tensor
//
//Parameters:
//	body: The rigidBody being scaled
//	scaleVector: A 3 DIM Vector comtaining the X, Y, and Z scale factors
void RigidBody_Scale(RigidBody* body, Vector* scaleVector)
{
	FrameOfReference_Scale(body->frame, scaleVector);
	RigidBody_SetInertiaOfCuboid(body);
}

///
//Sets the position of a rigidBody
//
//Parameters:
//  body: The rigidBody to set the position of
//  position: The position to move the body to
void RigidBody_SetPosition(RigidBody* body, Vector* position)
{
	FrameOfReference_SetPosition(body->frame, position);
}

///
//Sets the rotation of a rigidBody
//
//Parameters:
//  body: The rigidbody to set the rotation of
//  position: The rotation to set the body to
void RigidBody_SetRotation(RigidBody* body, Matrix* rotation)
{
	FrameOfReference_SetRotation(body->frame, rotation);
}
