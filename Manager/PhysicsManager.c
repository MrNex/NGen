#include "PhysicsManager.h"

#include <stdio.h>
#include <float.h>
#include <math.h>

#include "TimeManager.h"

//Internals
PhysicsBuffer* physicsBuffer = 0;

///
//Static Declarations

///
//Allocates memory for a new Physics Buffer
//
//Returns:
//      Pointer to a newly allocated physics buffer
static PhysicsBuffer* PhysicsManager_AllocateBuffer(void);

///
//Initializes a physics buffer
//
//Parameters:
//      buffer: The physics buffer to initialize
static void PhysicsManager_InitializeBuffer(PhysicsBuffer* buffer);

///
//Frees memory taken by a physics buffer
//
//Parameters:
//      buffer: The buffer to free the memory of
static void PhysicsManager_FreeBuffer(PhysicsBuffer* buffer);

///
//Resolves a collision
//
//Parameters:
//      collisions: The collision which needs to be resolved
static void PhysicsManager_ResolveCollision(struct Collision* collision);

///
//Determines if a collision needs to be resolved, or if it is resolving itself
//
//PArameters:
//      collision: The collision to test
//	collisionPoints: An array of pointers to vectors containing the collision points of object 1 and 2 respectively (in worldspace)
//
//REturns:
//      0 if no collision resolution is needed
//      1 if collision resolution is needed
static unsigned char PhysicsManager_IsResolutionNeeded(struct Collision* collision, Vector** collisionPoints);

///
//Takes two intersecting objects and restores them to the moment their surfaces touched
//
//Parameters:
//      collision: The collision to decouple
static void PhysicsManager_DecoupleCollision(struct Collision* collision);

///
//Determines the point of contact in a decoupled collision
//In the case where at least one of the objects is an AABB, there will be two separate collision points,
//One on each object.
//
//Parameters:
//      dest: Array of two Vectors to store the points of collision in. These are the collision points of 
//              obj1 and obj2 respectively.
//      collision: The collision to determine the point of contact 
static void PhysicsManager_DetermineCollisionPoints(Vector** dest, struct Collision* collision);

///
//Determines the point of contact on a sphere collider
//
//Parameters:
//      dest: A pointer to a vector to store the collision point in.
//      sphere: A pointer to the sphere collider data involved in collision
//      sphereFrame: A pointer to the frame of reference of the sphere involved in the collision
//      relativeMTV: A pointer to a vector representing the MTV !pointing towards the other object!
static void PhysicsManager_DetermineCollisionPointSphere(Vector* dest, const struct ColliderData_Sphere* sphere, const FrameOfReference* sphereFrame, const Vector* relativeMTV);

///
//Determines the point of contact on an AABB Collider
//For an AABB the point of contact will always be the center of mass to prevent rotation of the object
//
//Parameters:
//      dest: A pointer to a vector to store the collision point in
//      AABBFrame: The frame of reference of the AABB involved in the collision
static void PhysicsManager_DetermineCollisionPointAABB(Vector* dest, const FrameOfReference* AABBFrame);

///
//Determines the point of contact on a Convex Hull Collider
//
//PArameters:
//      dest: A pointer to a vector to store the collision point in
//      convexHull1: A pointer to the convex hull collider data involved in the collision which we are determining the point of collision on.
//      convexFrame1: A pointer to the frame of reference of the convex hull involved in the collision which we are determining the point of collision on.
//      convexHull2: A pointer to the other convex hull collider data involved in the collision, whether the object itself has a convex hull or not.
//      convexFrame2: A pointer to the frame of reference of the other convex hull involved in the collision, whether the object itself has a convex hull or not
//      relativeMTV: A pointer to a vector representing the MTV !Pointing towards the OTHER object (convexHull2)!
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
//      dest: A pointer to the vector to store the collision point in
//      furthestVertex: A pointer to the vector which registered as furthest in the direction of the MTV
//      frame: A pointer to the frame of reference of the convex hull which the furthestVertex belongs to
static void PhysicsManager_DetermineCollisionPointConvexHullVertex(Vector* dest, const Vector* furthestVertex, const FrameOfReference* frame);

///
//Determines the point of contact of a convex hull - convex hull collision when
//dealing with an Edge - Edge collision case. That is when both convex hulls have 
//exactly 2 vertices furthest in the direction of their respective MTVs
//
//Parameters:
//      dest: A pointer to the vector to store the collision point in
//      furthestOnHull1: A pointer to a dynamic array of vectors containing two vertices oriented in modelSpace on convexHull1 furthest in the direction of the respective relative MTV
//      convexFrame1: A pointer to the frame of reference of the convex hull which the vertices furthestOnHull1 belong to
//      furthestOnHull2: A pointer to a dynamic array of vectors containing two vertices oriented in ModelSpace on convexHull2 furthest in the direction of the respective relative MTV
//      convexFrame2: A pointer to the frame of reference of the convex Hull which the vertices furthestOnHull2 belong to
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
//      dest: A pointer to he vector to store the collision in
//      furthestOnHull1: A pointer to the dynamic array of vectors containing the vertices IN WORLDSPACE belonging to convexHull1 furthest in the direction of it's respective MTV
//      furthestOnHull2: A pointer to the dynamic array of vectors containing the vertices IN WORLDSPACE belongong to convexHull2 furthest in the direction of it's respective MTV
//	MTV: The minimum translation vector in the direction of convexHull1
static void PhysicsManager_DetermineCollisionPointConvexHullFace(Vector* dest, const DynamicArray* furthestOnHull1, const DynamicArray* furthestOnHull2, const Vector* MTV);

///
//Calculates and imparts the resulting collision impulse from the collision 
//
//Parameters:
//      collision: The collision having it's resulting impulses calculated and applied
//      pointsOfCollision: Array of 2 Vectors representing the point of collision in global space for obj1 and obj2 respectively
static void PhysicsManager_ApplyCollisionImpulses(struct Collision* collision, const Vector** pointsOfCollision);

///
//Calculates and applies the linear frictional forces when two objects slide against each other
//
//Parameters:
//      collision: The collision to apply linear frictional forces to
//      pointsOfCollision: An array of 2 pointers to vectors in 3 space representing the point of collision in global space for obj1 and obj2 respectively
//      staticCoefficient: The static coefficient of friction between the two colliding surfaces
//      dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
static void PhysicsManager_ApplyLinearFrictionalImpulses(struct Collision* collision, const float staticCoefficient, const float dynamicCoefficient, Vector** pointsOfCollision);

///
//Calculates and applies the angular frictional forces when two objects spin against each other
//
//Parameters:
//      collision: The collision to apply frictional torques to
//      staticCoefficient: The static coefficient of friction between the two colliding surfaces
//      dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
//	pointsOfCollision: An array of two vectors containing the points of collision for obj1 and obj2 respectively
static void PhysicsManager_ApplyFrictionalTorques(struct Collision* collision, const float staticCoefficient, const float dynamicCoefficient);

///
//Calculates and applies the rolling resistance between two objects
//
//Parameters:
//	collision: A pointer to the collision on which to compute rolling resistances
//	pointsOfCollision: An array of pointers to vectors containing the points of collision in worldspace for each object
//	resistanceCoefficient: The net rolling resistance coefficient between the two objects
static void PhysicsManager_ApplyRollingResistance(struct Collision* collision, Vector** pointsOfCollision, const float resistanceCoefficient);

///
//Implementations

///
//Allocates memory for a new Physics Buffer
//
//Returns:
//	Pointer to a newly allocated physics buffer
static PhysicsBuffer* PhysicsManager_AllocateBuffer()
{
	PhysicsBuffer* buffer = (PhysicsBuffer*)malloc(sizeof(PhysicsBuffer));
	return buffer;
}

///
//Initializes a physics buffer
//
//Parameters:
//	buffer: The physics buffer to initialize
static void PhysicsManager_InitializeBuffer(PhysicsBuffer* buffer)
{
	buffer->globalForces = LinkedList_Allocate();
	LinkedList_Initialize(buffer->globalForces);

	buffer->globalAccelerations = LinkedList_Allocate();
	LinkedList_Initialize(buffer->globalAccelerations);
}

///
//Frees memory taken by a physics buffer
//
//Parameters:
//	buffer: The buffer to free the memory of
static void PhysicsManager_FreeBuffer(PhysicsBuffer* buffer)
{
	//Delete all vectors in the list of global forces
	struct LinkedList_Node* current = buffer->globalForces->head;
	while(current != NULL)
	{
		Vector_Free((Vector*)current->data);
		current = current->next;
	}
	//Delete list of global forces
	LinkedList_Free(buffer->globalForces);

	//Delete all vectors in the list of global accelerations
	current = buffer->globalAccelerations->head;
	while(current != NULL)
	{
		Vector_Free((Vector*)current->data);
		current = current->next;
	}
	//Delete linked list of global accelerations
	LinkedList_Free(buffer->globalAccelerations);

	//Free the buffer itself
	free(buffer);
}

///
//Initializes the physics manager
void PhysicsManager_Initialize()
{
	//Allocate and initialize the buffer
	physicsBuffer = PhysicsManager_AllocateBuffer();
	PhysicsManager_InitializeBuffer(physicsBuffer);
}

///
//Frees internal data of the physics manager
void PhysicsManager_Free()
{
	PhysicsManager_FreeBuffer(physicsBuffer);
}

///
//Gets a reference to the internal physics buffer of the physics manager
//
//Returns:
//	A pointer to the physics buffer
PhysicsBuffer* PhysicsManager_GetPhysicsBuffer()
{
	return physicsBuffer;
}

///
//Adds a global force to the list of global forces
//
//Parameters:
//	force: A pointer to a vector of dimension 3 representing the force to add
void PhysicsManager_AddGlobalForce(Vector* force)
{
	LinkedList_Append(physicsBuffer->globalForces, force);
}

///
//Adds a global acceleration to the list of global accelerations
//
//Parameters:
//	acceleration: A pointer to a vector of dimension 3 representing the acceleration to add
void PhysicsManager_AddGlobalAcceleration(Vector* acceleration)
{
	LinkedList_Append(physicsBuffer->globalAccelerations, acceleration);
}

///
//Updates the Rigidbody components of all gameObjects
//
//Parameters:
//	gameObjects: THe linked list of gameobjects to update their rigidBodies
void PhysicsManager_Update(LinkedList* gameObjects)
{
	PhysicsManager_UpdateBodies(gameObjects);
	PhysicsManager_UpdateObjects(gameObjects);
}

///
//Updates the Rigidbody components of all gameObjects
//
//Parameters:
//	gameObjects: THe linked list of gameobjects to update their rigidBodies
void PhysicsManager_UpdateBodies(LinkedList* gameObjects)
{
	struct LinkedList_Node* current = gameObjects->head;
	struct LinkedList_Node* next = NULL;
	GObject* gameObject = NULL;

	float dt = TimeManager_GetDeltaSec();
	while(current != NULL)
	{
		next = current->next;
		gameObject = (GObject*)current->data;
		if(gameObject->body != NULL)
		{
			if( gameObject->body->physicsOn)
			{
				PhysicsManager_ApplyGlobals(gameObject->body);
				PhysicsManager_UpdateLinearPhysicsOfBody(gameObject->body, dt);
				PhysicsManager_UpdateRotationalPhysicsOfBody(gameObject->body, dt);
			}
		}


		current = next;

	}
}

///
//Applies all global forces to the given rigidbody
//
//Parameters:
//	body: The rigidbody to apply global forces to
void PhysicsManager_ApplyGlobals(RigidBody* body)
{
	struct LinkedList_Node* currentNode = physicsBuffer->globalForces->head;
	Vector* currentForce;

	//Apply each global force
	while(currentNode != NULL)
	{
		currentForce = (Vector*)currentNode->data;

		RigidBody_ApplyForce(body, (const Vector*)currentForce, &Vector_ZERO);

		currentNode = currentNode->next;
	}

	//If the object does not have an infinite mass
	if(body->inverseMass != 0.0f)
	{
		Vector scaledForce;
		Vector_INIT_ON_STACK(scaledForce, 3);

		//Apply each global acceleration
		currentNode = physicsBuffer->globalAccelerations->head;
		while(currentNode != NULL)
		{
			currentForce = (Vector*)currentNode->data;
			Vector_GetScalarProduct(&scaledForce, currentForce, 1.0f / body->inverseMass);
			
			RigidBody_ApplyForce(body, &scaledForce, &Vector_ZERO);
			
			currentNode = currentNode->next;
		}
	}
}

///
//Updates the linear physics of a rigidbody
//This determines calculations of acceleration, velocity, and position from netForce and netImpulse
//
//Parameters:
//	body: The body to update
//	dt: The change in time since last update
void PhysicsManager_UpdateLinearPhysicsOfBody(RigidBody* body, float dt)
{
	//F = MA
	//A = 1/M * F
	Vector_GetScalarProduct(body->acceleration, body->netForce, body->inverseMass);
	//J = MV so V = 1/M * J
	//J = J * 1/M
	//Will apply J to V later
	Vector_Scale(body->netImpulse, body->inverseMass);	


	Vector VT;
	Vector_INIT_ON_STACK( VT , 3 );
	Vector AT;
	Vector_INIT_ON_STACK( AT , 3);
	Vector VAT2;
	Vector_INIT_ON_STACK( VAT2 , 3);

	//Get V0T
	Vector_GetScalarProduct(&VT, body->velocity, dt);	//VT = V0 * dt		

	//Get AT
	Vector_GetScalarProduct(&AT, body->acceleration, dt);	//AT = A1 * dt

	//Get AT^2
	Vector_GetScalarProduct(&VAT2, &AT, dt);		//VAT2 = A1 * dt ^ 2
	//Get 1/2AT^2
	Vector_Scale(&VAT2, 0.5f);				//VAT2 = 1/2 * A1 * dt ^ 2

	//Get VT + 1/2AT^2
	Vector_Increment(&VAT2, &VT);				//VAT2 = VT + 1/2 * A1 * dt^2

	//X = X0 + V0T + 1/2AT^2
	Vector_Increment(body->frame->position, &VAT2);
	//V = V0 + AT
	Vector_Increment(body->velocity, &AT);	
	//V += 1/M * J
	Vector_Increment(body->velocity, body->netImpulse);
}

///
//Updates the rotational physics of a rigidbody
//This determines calculations of angular acceleration, angular velocity, and orientation / rotation
//from net torque and inverse Inertia
//
//Parameters:
//	body: The body to update
//	dt: The change in time since last update
void PhysicsManager_UpdateRotationalPhysicsOfBody(RigidBody* body, float dt)
{
	Vector AT;
	Vector_INIT_ON_STACK(AT, 3);

	Vector VT;
	Vector_INIT_ON_STACK(VT, 3);


	//T = IA
	//1/I * T = A
	Matrix_GetProductVector(body->angularAcceleration, body->inverseInertia, body->netTorque);
	//Same for instantaneous torque, accept it is applied directly to angular velocity at the end of this function
	Matrix_TransformVector(body->inverseInertia, body->netInstantaneousTorque);

	//A = dV / dT
	//A * dT = dV
	Vector_GetScalarProduct(&AT, body->angularAcceleration, dt);

	//A * dT = (V1 - V0)
	//A * dT + V0 = V1
	Vector_Increment(body->angularVelocity, &AT);
	//Apply instantaneousTorque to angularVelocity
	Vector_Increment(body->angularVelocity, body->netInstantaneousTorque);

	//V = dTheta / dt
	//V * dT = dTheta
	//V * dT = (Theta1 - Theta0)
	//V * dT + Theta0 = Theta1
	Vector_GetScalarProduct(&VT, body->angularVelocity, dt);

	//Rotate by |VT| around axis VT
	float theta = Vector_GetMag(&VT);
	Vector_Normalize(&VT);

	if(theta != 0)
	{
		FrameOfReference_Rotate(body->frame, &VT, theta);
	}

}

///
//Updates the Frame of reference component of all gameObjects to match their rigidbodies
//
//Parameters:
//	gameObjects: the linked list of gameObjects to update their rigidbodies
void PhysicsManager_UpdateObjects(LinkedList* gameObjects)
{
	float dt = TimeManager_GetDeltaSec();
	struct LinkedList_Node* current = gameObjects->head;
	struct LinkedList_Node* next = NULL;
	GObject* gameObject = NULL;


	while(current != NULL)
	{
		next = current->next;
		gameObject = (GObject*)current->data;
		if(gameObject->body != NULL)
		{
			if( gameObject->body->physicsOn)
			{
				Vector_Copy(gameObject->frameOfReference->position, gameObject->body->frame->position);
				Matrix_Copy(gameObject->frameOfReference->rotation, gameObject->body->frame->rotation);

				//Update previous net force
				Vector_GetScalarProduct(gameObject->body->previousNetForce, gameObject->body->netForce, dt);
				Vector_Increment(gameObject->body->previousNetForce, gameObject->body->netImpulse);

				//Update previous net torque
				Vector_GetScalarProduct(gameObject->body->previousNetTorque, gameObject->body->netTorque, dt);
				Vector_Increment(gameObject->body->previousNetTorque, gameObject->body->netInstantaneousTorque);

				//Set netforce back to 0
				Vector_Copy(gameObject->body->netForce, &Vector_ZERO);
				Vector_Copy(gameObject->body->acceleration, &Vector_ZERO);
				Vector_Copy(gameObject->body->netImpulse, &Vector_ZERO);
				Vector_Copy(gameObject->body->netTorque, &Vector_ZERO);
				Vector_Copy(gameObject->body->netInstantaneousTorque, &Vector_ZERO);
			}
		}


		current = next;

	}
}


///
//Resolves all collisions in a linked list
//
//Parameters:
//	collisions: A linked list of all collisions detected which need resolving
void PhysicsManager_ResolveCollisions(LinkedList* collisions)
{
	//Loop through the linked list of collisions
	struct LinkedList_Node* current = collisions->head;
	struct LinkedList_Node* next = NULL;

	struct Collision* collision;
	while(current != NULL)
	{
		next = current->next;
		collision = (struct Collision*)current->data;
		PhysicsManager_ResolveCollision(collision);



		current = next;


	}
}

///
//Resolves a collision
//
//Parameters:
//	collisions: The collision which needs to be resolved
static void PhysicsManager_ResolveCollision(struct Collision* collision)
{
	//Step 1: Use the minimum translation vector to pull the intersecting objects apart
	//If and only if the objects need to be pulled apart
	PhysicsManager_DecoupleCollision(collision);

	//Step 2: Calculate the point of collision
	//In the case that one of the objects is an AABB, there will be different collision points. Allocate an array of two vectors to store these points.
	Vector* pointsOfCollision[2];

	//Allocate and initialize these vectors individually
	pointsOfCollision[0] = Vector_Allocate();
	pointsOfCollision[1] = Vector_Allocate();

	Vector_Initialize(pointsOfCollision[0], 3);
	Vector_Initialize(pointsOfCollision[1], 3);

	PhysicsManager_DetermineCollisionPoints(pointsOfCollision, collision);

	//Vector_PrintTranspose(collision->minimumTranslationVector);
	//printf("%f\n", collision->overlap);

	//Check if the collision must be resolved
	if(PhysicsManager_IsResolutionNeeded(collision, pointsOfCollision))
	{

		//Step 3: Calculate and apply impulses due to collision
		PhysicsManager_ApplyCollisionImpulses(collision, (const Vector**)pointsOfCollision);


		//Step 4a: Calculate frictional coefficients
		float staticCoefficient = ((collision->obj1->body != NULL ? collision->obj1->body->staticFriction : 1.0f) + (collision->obj2->body != NULL ? collision->obj2->body->staticFriction : 1.0f)) / 2.0f;
		float dynamicCoefficient = ((collision->obj1->body != NULL ? collision->obj1->body->dynamicFriction : 1.0f) + (collision->obj2->body != NULL ? collision->obj2->body->dynamicFriction : 1.0f)) / 2.0f;

		//Step 4b: Calculate and apply frictional impulses
		PhysicsManager_ApplyLinearFrictionalImpulses(collision, staticCoefficient, dynamicCoefficient, pointsOfCollision);
		PhysicsManager_ApplyFrictionalTorques(collision, staticCoefficient, dynamicCoefficient);
		
		float rollingResistance = ((collision->obj1->body != NULL ? collision->obj1->body->rollingResistance : 0.0f) + (collision->obj2->body != NULL ? collision->obj2->body->rollingResistance : 1.0f)) * 0.5f;
		
		PhysicsManager_ApplyRollingResistance(collision, pointsOfCollision, rollingResistance);
	}
		

	//Free the vectors used to hold the collision points
	Vector_Free(pointsOfCollision[0]);
	Vector_Free(pointsOfCollision[1]);


}

///
//TODO: Account for linear velocity at closest point due to angular velocity.
//Determines if a collision needs to be resolved, or if it is resolving itself
//
//PArameters:
//	collision: The collision to test
//
//REturns:
//	0 if no collision resolution is needed
//	1 if collision resolution is needed
static unsigned char PhysicsManager_IsResolutionNeeded(struct Collision* collision, Vector** collisionPoints)
{
	//If the overlap is 0 (contact case) or negative, this collision does not need resolving
	if(fabs(collision->overlap) <= FLT_EPSILON)
	{
		return 0;	
	}

	//Find the relative velocity of the point of collision on obj2 from the point of collision on obj1
	Vector relativeVelocity;
	Vector_INIT_ON_STACK(relativeVelocity, 3);

	if(collision->obj2->body != NULL)
	{
		Vector totalVelocity2;
		Vector_INIT_ON_STACK(totalVelocity2, 3);

		Vector radius;
		Vector_INIT_ON_STACK(radius, 3);
		Vector_Subtract(&radius, collisionPoints[0], collision->obj2Frame->position);

		//RigidBody_CalculateLocalLinearVelocity(&totalVelocity2, collision->obj2->body, &radius);

		Vector_Increment(&totalVelocity2, collision->obj2->body->velocity);
		
		Vector_Increment(&relativeVelocity, &totalVelocity2);
	}

	if(collision->obj1->body != NULL)
	{
		Vector totalVelocity1;
		Vector_INIT_ON_STACK(totalVelocity1, 3);

		Vector radius;
		Vector_INIT_ON_STACK(radius, 3);
		Vector_Subtract(&radius, collisionPoints[0], collision->obj1Frame->position);

		//RigidBody_CalculateLocalLinearVelocity(&totalVelocity1, collision->obj1->body, &radius);

		Vector_Increment(&totalVelocity1, collision->obj1->body->velocity);
		
		Vector_Decrement(&relativeVelocity, &totalVelocity1);
	}

	//If the relative velocity is in the direction of the MTV object, object B is smashing into object A
	if(Vector_DotProduct(&relativeVelocity, collision->minimumTranslationVector) > 0.0f)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

///
//Takes two intersecting objects and restores them to the moment their surfaces touched
//
//Parameters:
//	collision: The collision to decouple
static void PhysicsManager_DecoupleCollision(struct Collision* collision)
{
	//If the overlap is 0 do not decouple
	if(collision->overlap <= FLT_EPSILON) return;

	//Step 0: Get the rigidbodies
	RigidBody* body1 = collision->obj1->body;
	RigidBody* body2 = collision->obj2->body;

	if(collision->obj1->collider->type == COLLIDER_SPHERE || collision->obj2->collider->type == COLLIDER_SPHERE)
	{
		//printf("MTV:\t\t");
		//Vector_PrintTranspose(collision->minimumTranslationVector);
		//printf("Overlap:\t%f\n", collision->overlap);
	}

	//Step 1: Determine the magnitude of the velocities in the direction of the MTV
	float mag1 = 0, mag2 = 0;
	if(body1 != NULL && !body1->freezeTranslation && body1->inverseMass != 0.0f)
	{
		mag1 = fabs(Vector_DotProduct(body1->velocity, collision->minimumTranslationVector));
	}

	if(body2 != NULL && !body2->freezeTranslation && body2->inverseMass != 0.0f)
	{
		mag2 = fabs(Vector_DotProduct(body2->velocity, collision->minimumTranslationVector));
	}

	//Step 2: Determine the ratio of each to the total
	float total = mag1 + mag2;
	if(total == 0.0f) return;
	float ratio1 = mag1 / total;
	float ratio2 = mag2 / total;

	float scale1 = ratio1 * collision->overlap;
	float scale2 = ratio2 * collision->overlap;

	//Step3 translate each object
	Vector resolutionVector;
	Vector_INIT_ON_STACK(resolutionVector, 3);
	if(body1 != NULL && !body1->freezeTranslation && body1->inverseMass != 0.0f)
	{
		//MTV always points toward obj1, so move obj1 along mtv
		Vector_GetScalarProduct(&resolutionVector, collision->minimumTranslationVector, scale1);
		FrameOfReference_Translate(body1->frame, &resolutionVector);
	}

	if(body2 != NULL && !body2->freezeTranslation && body2->inverseMass != 0.0f)
	{
		//MTV always points toward obj1, so move obj2 opposite mtv
		Vector_GetScalarProduct(&resolutionVector, collision->minimumTranslationVector, -scale2);
		FrameOfReference_Translate(body2->frame, &resolutionVector);
	}
}

///
//Determines the point of contact in a decoupled collision
//In the case where at least one of the objects is an AABB, there will be two separate collision points,
//One on each object.
//
//Parameters:
//	dest: Array of two Vectors to store the points of collision in. These are the collision points of 
//		obj1 and obj2 respectively.
//	collision: The collision to determine the point of contact 
static void PhysicsManager_DetermineCollisionPoints(Vector** dest, struct Collision* collision)
{
	unsigned char obj1PointFound = 0;	//Boolean dictating if obj1's collision point was determined
	unsigned char obj2PointFound = 0;	//Boolean dictating if obj2's collision point was determined

	//If either object is a sphere, first do the sphere test as it's the least expensive & most accurate
	if(collision->obj2->collider->type == COLLIDER_SPHERE)
	{
		PhysicsManager_DetermineCollisionPointSphere(dest[1], collision->obj2->collider->data->sphereData, collision->obj2Frame, collision->minimumTranslationVector);
		obj2PointFound = 1;

		//If obj2 is a sphere and obj1 is anything but an AABB, they have the same collision point
		if(collision->obj1->collider->type != COLLIDER_AABB)
		{
			Vector_Copy(dest[0], dest[1]);
			obj1PointFound = 1;
		}
	}
	else if(collision->obj1->collider->type == COLLIDER_SPHERE)
	{
		//Obj1's relative MTV is the negated MTV, because the MTV always points towards obj1 by convention
		Vector relativeMTV;
		Vector_INIT_ON_STACK(relativeMTV, 3);
		Vector_GetScalarProduct(&relativeMTV, collision->minimumTranslationVector, -1.0f);

		PhysicsManager_DetermineCollisionPointSphere(dest[0], collision->obj1->collider->data->sphereData, collision->obj1Frame, &relativeMTV);
		obj1PointFound = 1;

		//If obj1 is a sphere and obj2 is anything but an AABB, they have the same collision point
		if(collision->obj2->collider->type != COLLIDER_AABB)
		{
			Vector_Copy(dest[1], dest[0]);
			obj2PointFound = 1;
		}
	}

	//Exit here if both points have been found
	if(obj1PointFound && obj2PointFound)
	{
		return;
	}

	//Check if obj1 is an AABB
	if(collision->obj1->collider->type == COLLIDER_AABB)
	{
		PhysicsManager_DetermineCollisionPointAABB(dest[0], collision->obj1Frame);
		obj1PointFound = 1;
	}

	//Check if obj2 is an AABB
	if(collision->obj2->collider->type == COLLIDER_AABB)
	{
		PhysicsManager_DetermineCollisionPointAABB(dest[1], collision->obj2Frame);
		obj2PointFound = 1;
	}

	//Exit here if both points have been found
	if(obj1PointFound && obj2PointFound)
	{
		return;
	}

	//Only cases which involve 1 or more convex hulls & no spheres have not been determined yet.
	//If we have 1 of the points we have an AABB - Convex Hull case
	if(obj1PointFound || obj2PointFound)
	{
		//Determine which object has yet to have it's point found (This is the convex hull object)
		if(!obj1PointFound)
		{
			//If obj1 is convex we must convert obj2 to be convex as well.
			struct ColliderData_ConvexHull* AABBAsConvex = ConvexHullCollider_AllocateData();
			ConvexHullCollider_InitializeData(AABBAsConvex);

			AABBCollider_ToConvexHullCollider(AABBAsConvex, collision->obj2->collider->data->AABBData);

			//Next we must get obj1's relative MTV
			Vector relativeMTV;
			Vector_INIT_ON_STACK(relativeMTV, 3);
			Vector_GetScalarProduct(&relativeMTV, collision->minimumTranslationVector, -1.0f);

			//Now we can determine the collision point of obj1 using the convex hull method
			PhysicsManager_DetermineCollisionPointConvexHull(dest[0], 
				collision->obj1->collider->data->convexHullData, collision->obj1Frame, AABBAsConvex, collision->obj2Frame, 
				&relativeMTV);

			obj1PointFound = 1;

			//Free data allocated for representing obj2 as a convex hull
			ConvexHullCollider_FreeData(AABBAsConvex);
		}
		else
		{
			//If obj2 is convex we must convert obj1 to be convex as well
			struct ColliderData_ConvexHull* AABBAsConvex = ConvexHullCollider_AllocateData();
			ConvexHullCollider_InitializeData(AABBAsConvex);

			AABBCollider_ToConvexHullCollider(AABBAsConvex, collision->obj1->collider->data->AABBData);

			//Now we can determine the collision point of obj2 using the convex hull method
			PhysicsManager_DetermineCollisionPointConvexHull(dest[1], 
				collision->obj2->collider->data->convexHullData, collision->obj2Frame, AABBAsConvex, collision->obj1Frame, 
				collision->minimumTranslationVector);

			obj2PointFound = 1;

			//Free data allocated for representing obj1 as a convex hull
			ConvexHullCollider_FreeData(AABBAsConvex);
		}
	}
	//Else there is a convex hull - convex hull case
	else
	{
		//Grab the convex hull data from both colliders
		struct ColliderData_ConvexHull* convex1 = collision->obj1->collider->data->convexHullData;
		struct ColliderData_ConvexHull* convex2 = collision->obj2->collider->data->convexHullData;

		//Determine obj2's collision point
		PhysicsManager_DetermineCollisionPointConvexHull(dest[1], convex2, collision->obj2Frame, convex1, collision->obj1Frame, collision->minimumTranslationVector);
		obj2PointFound = 1;

		//Obj1's collision point will be the same
		Vector_Copy(dest[0], dest[1]);
		obj1PointFound = 1;
	}


}

///
//Determines the point of contact on a sphere collider
//
//Parameters:
//	dest: A pointer to a vector to store the collision point in.
//	sphere: A pointer to the sphere collider data involved in collision
//	sphereFrame: A pointer to the frame of reference of the sphere involved in the collision
//	relativeMTV: A pointer to a vector representing the MTV !pointing towards the other object!
static void PhysicsManager_DetermineCollisionPointSphere(Vector* dest, const struct ColliderData_Sphere* sphere, const FrameOfReference* sphereFrame, const Vector* relativeMTV)
{
	//GEt the scaled radius of the sphere collider
	float scaledRadius = SphereCollider_GetScaledRadius(sphere, sphereFrame);

	//Scale relative MTV by radius to find collision point in model space
	Vector_GetScalarProduct(dest, relativeMTV, scaledRadius);

	//Translate collision point into worldspace to get final collision point.
	Vector_Increment(dest, sphereFrame->position);
}

///
//Determines the point of contact on an AABB Collider
//For an AABB the point of contact will always be the center of mass to prevent rotation of the object
//
//Parameters:
//	dest: A pointer to a vector to store the collision point in
//	AABBFrame: The frame of reference of the AABB involved in the collision
static void PhysicsManager_DetermineCollisionPointAABB(Vector* dest, const FrameOfReference* AABBFrame)
{
	//The collisiion point on an object which must prevent from rotating must be the center of mass in worldspace
	Vector_Copy(dest, AABBFrame->position);
}

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
	const Vector* relativeMTV)
{
	//Create an unsigned character to serve as a boolean for whether the collision point was found yet
	unsigned char found = 0;

	//allocate arrays of vectors to hold the model space oriented points of colliders
	Vector** modelOrientedPoints1 = (Vector**)malloc(sizeof(Vector*) * convexHull1->points->size);
	Vector** modelOrientedPoints2 = (Vector**)malloc(sizeof(Vector*) * convexHull2->points->size);

	//Allocate & initialize individual vectors in array
	for(unsigned int i = 0; i < convexHull1->points->size; i++)
	{
		modelOrientedPoints1[i] = Vector_Allocate();
		Vector_Initialize(modelOrientedPoints1[i], 3);
	}

	//Allocate & initialize individual vectors in array
	for(unsigned int i = 0; i < convexHull2->points->size; i++)
	{
		modelOrientedPoints2[i] = Vector_Allocate();
		Vector_Initialize(modelOrientedPoints2[i], 3);
	}


	//Get the points of the collider oriented in modelSpace
	ConvexHullCollider_GetOrientedModelPoints(modelOrientedPoints1, convexHull1, convexFrame1);
	ConvexHullCollider_GetOrientedModelPoints(modelOrientedPoints2, convexHull2, convexFrame2);


	//We must now determine the type of collision:
	//Point - Point / Edge / Face
	//Edge - Edge
	//Edge - Face
	//OR Face - Face
	//
	//We can do this by finding the number of modelOriented points furthest in the direction of the respective relative MTV for each object
	//Allocate & Initialize Dynamic arrays to hold these points
	DynamicArray* furthestPoints1 = DynamicArray_Allocate();
	DynamicArray* furthestPoints2 = DynamicArray_Allocate();

	DynamicArray_Initialize(furthestPoints1, sizeof(Vector));
	DynamicArray_Initialize(furthestPoints2, sizeof(Vector));

	ConvexHullCollider_GetFurthestPoints(furthestPoints1, convexHull1, (const Vector**)modelOrientedPoints1, relativeMTV);

	//If only a single closest point was found, We have a special case of:
	//	Vertex - Vertex / Edge / Face
	if(furthestPoints1->size == 1)
	{
		//Get the furthest point from the dynamic array
		Vector* furthestPoint = (Vector*)DynamicArray_Index(furthestPoints1, 0);
		//Calculate collision point of vertex - Vertex / edge / face case
		PhysicsManager_DetermineCollisionPointConvexHullVertex(dest, furthestPoint, convexFrame1);
		//Collision point was determined, flag found
		found = 1;
	}

	//If the collision point was not yet determined
	if(!found)
	{
		Vector relativeMTVForObj2;
		Vector_INIT_ON_STACK(relativeMTVForObj2, 3);
		Vector_GetScalarProduct(&relativeMTVForObj2, relativeMTV, -1.0f);
		ConvexHullCollider_GetFurthestPoints(furthestPoints2, convexHull2, (const Vector**)modelOrientedPoints2, &relativeMTVForObj2);


		//If only a single furthest point was found, We have a special case of:
		//	Vertex - Vertex / Edge / Face
		if(furthestPoints2->size == 1)
		{
			//Get the furthest point from the dynamic array
			Vector* furthestPoint = (Vector*)DynamicArray_Index(furthestPoints2, 0);
			//Calculate collision point of Vertex - Vertex / Edge / Face case
			PhysicsManager_DetermineCollisionPointConvexHullVertex(dest, furthestPoint, convexFrame2);
			//Collision point was determined, flag found
			found = 1;
		}

	}


	//If the collision point has still yet to be found
	if(!found)
	{
		//If both objects have 2 furthest points we have a special case of:
		//	Edge - Edge
		if(furthestPoints1->size == 2 && furthestPoints2->size == 2)
		{
			//Caculate collision point of Edge - Edge case
			PhysicsManager_DetermineCollisionPointConvexHullEdge(dest, furthestPoints1, convexFrame1, furthestPoints2, convexFrame2);
			//Collision point wasdetermined, flag found
			found = 1;
		}
	}

	//If th collision point has still yet to be found
	if(!found)
	{
		//It must be an Edge / Face - Face case

		//In this case we must translate all points into worldspace before calling the collision point determination function!
		for(unsigned int i = 0; i < furthestPoints1->size; i++)
		{
			Vector* current = (Vector*)DynamicArray_Index(furthestPoints1, i);
			Vector_Increment(current, convexFrame1->position);
		}

		for(unsigned int i = 0; i < furthestPoints2->size; i++)
		{
			Vector*current = (Vector*)DynamicArray_Index(furthestPoints2, i);
			Vector_Increment(current, convexFrame2->position);
		}

		//Calculate approximate collision point of general case
		PhysicsManager_DetermineCollisionPointConvexHullFace(dest, furthestPoints1, furthestPoints2, relativeMTV);
		//Collision point was determined, flag found
		found = 1;
	}

	//Delete arrays of modelSpace oriented collider points
	for(unsigned int i = 0; i < convexHull1->points->size; i++)
	{
		Vector_Free(modelOrientedPoints1[i]);
	}
	free(modelOrientedPoints1);
	for(unsigned int i = 0; i < convexHull2->points->size; i++)
	{
		Vector_Free(modelOrientedPoints2[i]);
	}
	free(modelOrientedPoints2);


	//Delete dynamic arrays of furthest points
	DynamicArray_Free(furthestPoints1);
	DynamicArray_Free(furthestPoints2);
}

///
//Determines the point of contact of a Convex Hull - Convex Hull collision when
//dealing with a Vertex on Vertex / Edge / Face collision case. That is when
//One convex hull has only a single vertex furthest in the direction of a minimum translation vector
//
//Parameters:
//	dest: A pointer to the vector to store the collision point in
//	furthestVertex: A pointer to the vector which registered as furthest in the direction of the MTV
//	frame: A pointer to the frame of reference of the convex hull which the furthestVertex belongs to
static void PhysicsManager_DetermineCollisionPointConvexHullVertex(Vector* dest, const Vector* furthestVertex, const FrameOfReference* frame)
{
	//If only one vertex is furthest in the direction of the relative MTV, this vertex translated into world space
	//Must be the point of collision!
	Vector_Add(dest, furthestVertex, frame->position);
}

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
	const DynamicArray* furthestOnHull2, const FrameOfReference* convexFrame2)
{
	//We can represent the two points on each convex hull as a line, then test for the intersection of the two lines
	Vector direction1;	//Will store Direction of line on convexHull1
	Vector direction2;	//Will store Direction of line on convexHull2

	Vector offset1;		//Will store offset of line on convex hull 1 from origin of worldspace
	Vector offset2;		//Will store offset of line on convex hull 2 from origin of worldspace

	//Initialize directions & offsets
	Vector_INIT_ON_STACK(direction1, 3);
	Vector_INIT_ON_STACK(direction2, 3);
	Vector_INIT_ON_STACK(offset1, 3);
	Vector_INIT_ON_STACK(offset2, 3);

	//Determine line directions
	//Const modifier is removed from dynamic arrays but data will not be altered in this function.
	Vector_Subtract(&direction1, (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 1), (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 0));
	Vector_Subtract(&direction2, (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull2, 1), (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull2, 0));

	//Determine line offsets from orign of worldspace
	//Const modifier is removed from dynamic arrays but data will not be altered in this function.
	Vector_Add(&offset1, convexFrame1->position, (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 0));
	Vector_Add(&offset2, convexFrame2->position, (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull2, 0));

	//Solve for t
	//offset1 + t*direction1 = offset2 + t*direction2
	//offset1 - offset2 = t * (direction2 - direction1)
	//offsetFinal = t * directionFinal
	Vector offsetFinal;
	Vector directionFinal;

	Vector_INIT_ON_STACK(offsetFinal, 3);
	Vector_INIT_ON_STACK(directionFinal, 3);

	//offsetFinal = offset1 - offset2
	Vector_Subtract(&offsetFinal, &offset1, &offset2);
	//directionFinal = direction2 - direction1
	Vector_Subtract(&directionFinal, &direction2, &direction1);

	//TODO: Run a test to make sure all solutions for t are equal
	//We now have a system of equations for 3 solutions for t. We should solve for one which has a nonzero directionFinal component
	float t = 0.0f;
	for(int i = 0; i < 3; i++)
	{
		if(directionFinal.components[i] != 0.0f)
		{
			//t = offsetFinal / directionFinal
			t = offsetFinal.components[i] / directionFinal.components[i];
			break;
		}
	}

	//Now we can use either of the equations for the lines to solve for the collision point!
	//dest = offset1 + t * direction1
	Vector_GetScalarProduct(dest, &direction1, t);
	Vector_Increment(dest, &offset1);
}

///
//Determines the point of contact of a convex hull - convex hull collision when
//dealing with an Edge / Face - Face collision case. This is when both convex hulls have
// > 1 vertex furthest in the direction of their respective MTVs AND one convex hull has > 2
//vertices furthest in the direction of it's respective MTV
//
//Parameters:
//	dest: A pointer to he vector to store the collision in
//	furthestOnHull1: A pointer to the dynamic array of vectors containing the vertices belonging to convexHull1 furthest in the direction of it's respective MTV
//	furthestOnHull2: A pointer to the dynamic array of vectors containing the vertices belongong to convexHull2 furthest in the direction of it's respective MTV
//	MTV: The minimum translation vector pointing toward convexHull1 by convention
static void PhysicsManager_DetermineCollisionPointConvexHullFace(Vector* dest, const DynamicArray* furthestOnHull1, const DynamicArray* furthestOnHull2, const Vector* MTV)
{

	//We must construct two axes which, along with the MTV, form a basis for 3-space.
	//These two axes will form a basis for the collision plane.
	Vector x;
	Vector y;

	Vector_INIT_ON_STACK(x, 3);
	Vector_INIT_ON_STACK(y, 3);

	Vector proj;
	Vector sum;
	Vector_INIT_ON_STACK(proj, 3);
	Vector_INIT_ON_STACK(sum, 3);
	//Perform the Graham Schmidt process to form a basis for 3-space from the MTV
	//First let X be the Perpendicular component of the X axis with respect to the MTV
	Vector_GetProjection(&proj, &Vector_E1, MTV);
	Vector_Subtract(&x, &Vector_E1, &proj);

	//Make sure x is not the zero vector
	if(Vector_DotProduct(&x, &x) < FLT_EPSILON)
	{
		//Let X be the perpendicular component of the Z axis with respect to the MTV
		Vector_GetProjection(&proj, &Vector_E3, MTV);
		Vector_Subtract(&x, &Vector_E3, &proj);

	}

	//Let y be the perpendicular component of the Y xis with respect to the subspace spanned by {MTV, x}
	Vector_GetProjection(&sum, &Vector_E2, MTV);
	Vector_GetProjection(&proj, &Vector_E2, &x);
	Vector_Increment(&sum, &proj);

	Vector_Subtract(&y, &Vector_E2, &sum);

	//Make sure y is not the zero vector
	if(Vector_DotProduct(&y, &y) < FLT_EPSILON)
	{
		//Let y be the perpendicular component of the Z axis with respect to the subspace spanned by MTV and x
		Vector_GetProjection(&sum, &Vector_E3, MTV);
		Vector_GetProjection(&proj, &Vector_E3, &x);
		Vector_Increment(&sum, &proj);

		Vector_Subtract(&y, &Vector_E3, &sum);
	}

	//Now that we have a basis for the collision plane, we can determine the span of overlap on each axis of both sets of points.
	float min1, max1, min2, max2;

	//Start with axis x	
	Vector* current = (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 0);
	min1 = max1 = Vector_DotProduct(current, &x);

	current = (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull2, 0);
	min2 = max2 = Vector_DotProduct(current, &x);

	for(unsigned int i = 1; i < furthestOnHull1->size; i++)
	{
		current = DynamicArray_Index((DynamicArray*)furthestOnHull1, i);
		float dotProd = Vector_DotProduct(current, &x);

		if(dotProd < min1) min1 = dotProd;
		else if(dotProd > max1) max1 = dotProd;
	}

	for(unsigned int i = 1; i < furthestOnHull2->size; i++)
	{
		current = DynamicArray_Index((DynamicArray*)furthestOnHull2, i);
		float dotProd = Vector_DotProduct(current, &x);

		if(dotProd < min2) min2 = dotProd;
		else if(dotProd > max2) max2 = dotProd;
	}

	//First we must order the points from lowest to highest, but we only care about the middle 2.
	float lMid, uMid;
	lMid = min1 < min2 ?  min2 : min1;
	uMid = max1 > max2 ? max2 : max1;

	//Now we must find the scalar midpoint of the projections on this axis.
	float mid = lMid + 0.5f * (uMid - lMid);

	//Set the "x" component of the point of collision
	Vector_GetScalarProduct(dest, &x, mid);

	//Next do the y axis	
	current = (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 0);
	min1 = max1 = Vector_DotProduct(current, &y);

	current = (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull2, 0);
	min2 = max2 = Vector_DotProduct(current, &y);

	for(unsigned int i = 1; i < furthestOnHull1->size; i++)
	{
		current = DynamicArray_Index((DynamicArray*)furthestOnHull1, i);
		float dotProd = Vector_DotProduct(current, &y);

		if(dotProd < min1) min1 = dotProd;
		else if(dotProd > max1) max1 = dotProd;
	}

	for(unsigned int i = 1; i < furthestOnHull2->size; i++)
	{
		current = DynamicArray_Index((DynamicArray*)furthestOnHull2, i);
		float dotProd = Vector_DotProduct(current, &y);

		if(dotProd < min2) min2 = dotProd;
		else if(dotProd > max2) max2 = dotProd;
	}

	//First we must order the points from lowest to highest
	lMid = min1 < min2 ?  min2 : min1;
	uMid = max1 > max2 ? max2 : max1;

	//Now we must find the scalar midpoint of the projections on this axis.
	mid = lMid + 0.5f * (uMid - lMid);

	//Set the "y" component of the point of collision
	Vector_GetScalarProduct(&proj, &y, mid);
	Vector_Increment(dest, &proj);

	//Perform the final component in the direction of the collision normal
	current = (Vector*)DynamicArray_Index((DynamicArray*)furthestOnHull1, 0);
	mid = Vector_DotProduct(current, MTV);
	//Set the final component
	Vector_GetScalarProduct(&proj, MTV, mid);
	Vector_Increment(dest, &proj);	

}

///
//Calculates and imparts the resulting collision impulse from the collision
//
//Parameters:
//	collision: The collision having it's resulting impulses calculated and applied
//	pointOfCollision: Vector representing the point of collision in global space
static void PhysicsManager_ApplyCollisionImpulses(struct Collision* collision, const Vector** pointsOfCollision)
{
	//Calculation for the numerator of Chris Hecker's collision impulse equation
	//Hardcode coefficients of restitution for now
	float coefficientOfRestitution1 = collision->obj1->body != NULL ? collision->obj1->body->coefficientOfRestitution : 1.0f;
	float coefficientOfRestitution2 = collision->obj2->body != NULL ? collision->obj2->body->coefficientOfRestitution : 1.0f;

	//TODO: Find out if the final coefficient e is the product of the two coefficients of restitution
	float e = coefficientOfRestitution1 * coefficientOfRestitution2;

	Vector radP1;	//Radial vector from CoM of obj1 to point of collision
	Vector radP2;	//Radial vector from CoM of obj2 to point of collision

	Vector_INIT_ON_STACK(radP1, 3);
	Vector_INIT_ON_STACK(radP2, 3);

	Vector_Subtract(&radP1, pointsOfCollision[0], collision->obj1Frame->position);
	Vector_Subtract(&radP2, pointsOfCollision[1], collision->obj2Frame->position);


	Vector velP1;	//total Linear Velocity at point P on obj1
	Vector velP2;	//Total linear velocity at point P on obj2

	Vector_INIT_ON_STACK(velP1, 3);
	Vector_INIT_ON_STACK(velP2, 3);

	if(collision->obj1->body != NULL)
	{
		Vector_CrossProduct(&velP1, collision->obj1->body->angularVelocity, &radP1);
		Vector_Increment(&velP1, collision->obj1->body->velocity);
	}
	else
	{
		Vector_Copy(&velP1, &Vector_ZERO);
	}

	if(collision->obj2->body != NULL)
	{
		Vector_CrossProduct(&velP2, collision->obj2->body->angularVelocity, &radP2);
		Vector_Increment(&velP2, collision->obj2->body->velocity);
	}
	else
	{
		Vector_Copy(&velP2, &Vector_ZERO);
	}

	Vector velAB;	//Relative linear velocity of point P on object A from an observer on Point P on obj B
	Vector_INIT_ON_STACK(velAB, 3);

	Vector_Subtract(&velAB, &velP1, &velP2);

	//Get the magnitude of the relative velocity in the direction of obj1 from the point of contact
	float relNormalVel = Vector_DotProduct(&velAB, collision->minimumTranslationVector);


	//The final numerator of the impulse equation
	float numerator =  (-1 - e) * relNormalVel;

	//Calculation for Denominator of Chris Hecker's impulse equation
	Vector torque1;	//Torque due to collision on obj1
	Vector torque2;	//Torque due to collision on obj2

	Vector_INIT_ON_STACK(torque1, 3);
	Vector_INIT_ON_STACK(torque2, 3);

	Vector_CrossProduct(&torque1, &radP1, collision->minimumTranslationVector);
	Vector_CrossProduct(&torque2, &radP2, collision->minimumTranslationVector);


	Vector velPFromT1;	//Linear velocity of point P on obj1 due to torque of obj1
	Vector velPFromT2;	//Linear velocity of point P on Obj2 due to torque of obj2

	Vector_INIT_ON_STACK(velPFromT1, 3);
	Vector_INIT_ON_STACK(velPFromT2, 3);

	if(collision->obj1->body != NULL && collision->obj1->body->inverseMass != 0.0f)
	{
		Matrix inertiaInWorldSpace;
		Matrix_INIT_ON_STACK(inertiaInWorldSpace, 3, 3);

		RigidBody_CalculateMomentOfInertiaInWorldSpace(&inertiaInWorldSpace, collision->obj1->body);

		Matrix inverseInertia;
		Matrix_INIT_ON_STACK(inverseInertia, 3, 3);
		Matrix_GetInverse(&inverseInertia, &inertiaInWorldSpace);

		//Calculate angular acceleration due to torque
		Matrix_TransformVector(&inverseInertia, &torque1);

		//Determine linear velocity of pont P on obj1 due to angular acceleration of obj1
		Vector_CrossProduct(&velPFromT1, &torque1, &radP1);
	}
	else
	{
		Vector_Copy(&velPFromT1, &Vector_ZERO);
	}

	if(collision->obj2->body != NULL && collision->obj2->body->inverseMass != 0.0f)
	{
		Matrix inertiaInWorldSpace;
		Matrix_INIT_ON_STACK(inertiaInWorldSpace, 3, 3);

		RigidBody_CalculateMomentOfInertiaInWorldSpace(&inertiaInWorldSpace, collision->obj2->body);

		Matrix inverseInertia;
		Matrix_INIT_ON_STACK(inverseInertia, 3, 3);
		Matrix_GetInverse(&inverseInertia, &inertiaInWorldSpace);

		//Calculate angular acceleration due to torque
		Matrix_TransformVector(&inverseInertia, &torque2);

		//Determine linear velocity of pont P on obj1 due to angular acceleration of obj1
		Vector_CrossProduct(&velPFromT2, &torque2, &radP2);
	}
	else
	{
		Vector_Copy(&velPFromT2, &Vector_ZERO);
	}

	//Add the velocities due to torques together
	Vector_Increment(&velPFromT1, &velPFromT2);


	//Get the sum of the inverse masses
	float iMassSum = 0.0f;
	if(collision->obj1->body != NULL)
	{
		iMassSum += collision->obj1->body->inverseMass;
	}

	if(collision->obj2->body != NULL)
	{
		iMassSum += collision->obj2->body->inverseMass;
	}

	//Final calculation for denominator of impulse equation
	float denominator = iMassSum + Vector_DotProduct(&velPFromT1, collision->minimumTranslationVector);

	//Calculate impulse
	float impulse = numerator/denominator;
	collision->resolutionImpulse = impulse;

	Vector impulseVector;
	Vector_INIT_ON_STACK(impulseVector, 3);
	//V1After = V1Before + impulse / M1 * MTV
	Vector_GetScalarProduct(&impulseVector, collision->minimumTranslationVector, impulse);
	//Apply impulse
	if(collision->obj1->body != NULL && collision->obj1->body->inverseMass != 0.0f)
	{
		RigidBody_ApplyImpulse(collision->obj1->body, &impulseVector, &radP1);
	}
	if(collision->obj2->body != NULL && collision->obj2->body->inverseMass != 0.0f)
	{
		//V2After = V2Before - impulse / M2 * MTV
		Vector_Scale(&impulseVector, -1.0f);

		RigidBody_ApplyImpulse(collision->obj2->body, &impulseVector, &radP2);

	}

}

///
//Calculates and applies the frictional forces when two objects slide against each other
//Algorithm is given by:
//	http://en.wikipedia.org/wiki/Collision_response#Impulse-Based_Friction_Model
//
//Parameters:
//	collision: The collision to apply frictional forces to
//	pointsOfCollision: An array of 2 pointers to vectors in 3 space representing the point of collision in global space for obj1 and obj2 respectively
//	staticCoefficient: The static coefficient of friction between the two colliding surfaces
//	dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
static void PhysicsManager_ApplyLinearFrictionalImpulses(struct Collision* collision, const float staticCoefficient, const float dynamicCoefficient, Vector** pointsOfCollision)
{
	//Step 1) Find the direction of a tangent vector which is tangential to the surface of collision in the direction of movement / applied forces if there is no tangential movement
	Vector unitTangentVector;
	Vector_INIT_ON_STACK(unitTangentVector, 3);

	Vector relativeVelocity;
	Vector_INIT_ON_STACK(relativeVelocity, 3);

	//Find relative velocity of the point of collision on object2 from observer on the point of collsion on object1
	if(collision->obj2->body != NULL)
	{
		Vector_Copy(&relativeVelocity, collision->obj2->body->velocity);
		
		Vector localVelocityAtCollisionPoint;
		Vector radius;
		Vector_INIT_ON_STACK(localVelocityAtCollisionPoint, 3);
		Vector_INIT_ON_STACK(radius, 3);

		Vector_Subtract(&radius, pointsOfCollision[1], collision->obj2->body->frame->position);
		RigidBody_CalculateLocalLinearVelocity(&localVelocityAtCollisionPoint, collision->obj2->body, &radius);

		Vector_Increment(&relativeVelocity, &localVelocityAtCollisionPoint);
	}
	if(collision->obj1->body != NULL)
	{
		Vector_Decrement(&relativeVelocity, collision->obj1->body->velocity);

		Vector localVelocityAtCollisionPoint;
		Vector radius;
		Vector_INIT_ON_STACK(localVelocityAtCollisionPoint, 3);
		Vector_INIT_ON_STACK(radius, 3);

		Vector_Subtract(&radius, pointsOfCollision[0], collision->obj1->body->frame->position);
		RigidBody_CalculateLocalLinearVelocity(&localVelocityAtCollisionPoint, collision->obj1->body, &radius);

		Vector_Decrement(&relativeVelocity, &localVelocityAtCollisionPoint);
	}

	//Make sure the relative velocity is nonZero
	if(Vector_GetMag(&relativeVelocity) > FLT_EPSILON)
	{
		//If there is relative movement between the objects we must figure out the component of this relative velocity which is tangent to the surface
		Vector relativeVelocityPerp;
		Vector_INIT_ON_STACK(relativeVelocityPerp, 3);


		//Project the relative velocity onto the surface normal
		//Vector_GetScalarProduct(&relativeVelocityPerp, &relativeVelocity, Vector_DotProduct(&relativeVelocity, collision->minimumTranslationVector));
		Vector_GetProjection(&relativeVelocityPerp, &relativeVelocity, collision->minimumTranslationVector);

		//VTangential = V - VPerpendicular
		Vector_Subtract(&unitTangentVector, &relativeVelocity, &relativeVelocityPerp);
		Vector_Normalize(&unitTangentVector);
	}

	//if the unit tangent vector is still 0
	if(Vector_GetMag(&unitTangentVector) <= FLT_EPSILON)
	{
		//We must use the relative net force, the net force applied to rigidbody 2 relative to rigidbody1
		Vector relativeNetForce;
		Vector_INIT_ON_STACK(relativeNetForce, 3);


		if(collision->obj2->body != NULL)
		{
			Vector_Increment(&relativeNetForce, collision->obj2->body->previousNetForce);
		}
		if(collision->obj1->body != NULL)
		{
			Vector_Decrement(&relativeNetForce, collision->obj1->body->previousNetForce);
		}

		//Project the net external force onto the surface normal
		Vector relNetForcePerp;
		Vector_INIT_ON_STACK(relNetForcePerp, 3);

		//Vector_GetScalarProduct(&cumNetForcePerp, &cumulativeNetForce, Vector_DotProduct(&cumulativeNetForce, collision->minimumTranslationVector));
		Vector_GetProjection(&relNetForcePerp, &relativeNetForce, collision->minimumTranslationVector);


		//VTangential = V - VPerpendicular
		Vector_Subtract(&unitTangentVector, &relativeNetForce, &relNetForcePerp);
		Vector_Normalize(&unitTangentVector);
	}

	//Step 2) Compute the static and dynamic frictional force magnitudes based off of the magnitude of the
	//component of the reaction impulse of the collision in the direction of the contact normal
	float reactionMag = collision->resolutionImpulse;
	/*
	if(collision->obj1->body != NULL && collision->obj1->body->inverseMass != 0.0f && !collision->obj1->body->freezeTranslation)
	{
		reactionMag = fabs(Vector_DotProduct(collision->obj1->body->netImpulse, collision->minimumTranslationVector));
	}
	else
	{
		reactionMag = fabs(Vector_DotProduct(collision->obj2->body->netImpulse, collision->minimumTranslationVector));
	}*/

	float staticMag = staticCoefficient * reactionMag;
	float dynamicMag = dynamicCoefficient * reactionMag;

	//Step 3) Compute & apply the frictional impulse
	//If none of the relative velocity is in the direction of the tangent vector (The object is not moving) 
	//Or the impulse which would have caused the velocity-- in the direction of the tangent vector is less than the static mag
	float relVelocityTangentialMag = Vector_DotProduct(&relativeVelocity, &unitTangentVector);

	float relImpulseTangentialMag1;
	float relImpulseTangentialMag2;
	if(collision->obj1->body != NULL && collision->obj1->body->inverseMass != 0.0f && !collision->obj1->body->freezeTranslation)
	{
		relImpulseTangentialMag1 = relVelocityTangentialMag / collision->obj1->body->inverseMass;
		
		Vector frictionalImpulse;
		Vector_INIT_ON_STACK(frictionalImpulse, 3);

		Vector radius;
		Vector_INIT_ON_STACK(radius, 3);
		
		if(!collision->obj1->body->freezeRotation)
		{
			Vector_Subtract(&radius, pointsOfCollision[0], collision->obj1->body->frame->position);

		}

		if(relImpulseTangentialMag1 <= staticMag)
		{
			Vector_GetScalarProduct(&frictionalImpulse, &unitTangentVector, relImpulseTangentialMag1);
		}
		else
		{
			Vector_GetScalarProduct(&frictionalImpulse, &unitTangentVector, dynamicMag);	
		}

		//Apply linear friction
		RigidBody_ApplyImpulse(collision->obj1->body, &frictionalImpulse, &radius);

	}
	if(collision->obj2->body != NULL && collision->obj2->body->inverseMass != 0.0f)
	{
		relImpulseTangentialMag2 = relVelocityTangentialMag / collision->obj2->body->inverseMass;

		Vector frictionalImpulse;
		Vector_INIT_ON_STACK(frictionalImpulse, 3);

		Vector radius;
		Vector_INIT_ON_STACK(radius, 3);


		if(!collision->obj2->body->freezeRotation)
		{
			Vector_Subtract(&radius, pointsOfCollision[1], collision->obj2->body->frame->position);
		}

		if(relImpulseTangentialMag2 <= staticMag)
		{
			Vector_GetScalarProduct(&frictionalImpulse, &unitTangentVector, -relImpulseTangentialMag2);
		}
		else
		{
			Vector_GetScalarProduct(&frictionalImpulse, &unitTangentVector, -dynamicMag);
		}


		RigidBody_ApplyImpulse(collision->obj2->body, &frictionalImpulse, &radius);

	}


}

///
//Calculates and applies the angular frictional forces when two objects spin against each other
//
//Parameters:
//	collision: The collision to apply frictional torques to
//	staticCoefficient: The static coefficient of friction between the two colliding surfaces
//	dynamicCoefficient: The dynamic coefficient of friction between the two colliding surfaces
//	pointsOfCollision: An array of two vectors containing the points of collision for object 1 and object 2 respectively
static void PhysicsManager_ApplyFrictionalTorques(struct Collision* collision, const float staticCoefficient, const float dynamicCoefficient)
{

	//Step 0) Save references to both bodies to reduce typing and increase readability
	RigidBody* body1 = collision->obj1->body;
	RigidBody* body2 = collision->obj2->body;

	//Step 1) Find the relative angular velocity of obj2 from the center of mass of obj1
	Vector aVRel;
	Vector_INIT_ON_STACK(aVRel, 3);
	if(body2 != NULL && body2->inverseMass != 0.0f && !body2->freezeRotation)
	{
		Vector_Increment(&aVRel, body2->angularVelocity);
	}

	if(body1 != NULL && body1->inverseMass != 0.0f && !body1->freezeRotation)
	{
		Vector_Decrement(&aVRel, body1->angularVelocity);
	}

	//Step 2: Find the magnitude of force of the collision normal to the colliding surface
	float fNormal = 0.0f;
	if(body1 != NULL && body1->inverseMass != 0.0f && !body1->freezeRotation)
	{
		fNormal = fabs(Vector_DotProduct(body1->netImpulse, collision->minimumTranslationVector));
	}
	else
	{
		fNormal = fabs(Vector_DotProduct(body2->netImpulse, collision->minimumTranslationVector));
	}

	//Step 3: Use normal force to determine the static and dynamic friction magnitudes
	float staticMag = fNormal * staticCoefficient;
	float dynamicMag = fNormal * dynamicCoefficient;

	//Step 4: Determine the magnitude of the relative angular velocity in the direction of the surface normal
	float relAVPerp = Vector_DotProduct(&aVRel, collision->minimumTranslationVector);

	//Step 5: Apply the frictional torques to each object
	//Object 1
	if(body1 != NULL && body1->inverseMass != 0.0f && !body1->freezeRotation)
	{
		//Step a: Compute the angular impulse / momentum in the direction of surface normal
		Vector lA;
		Vector_INIT_ON_STACK(lA, 3);
		
		Matrix iA;
		Matrix_INIT_ON_STACK(iA, 3, 3);
		RigidBody_CalculateMomentOfInertiaInWorldSpace(&iA, body1);

		Vector_GetScalarProduct(&lA, collision->minimumTranslationVector, relAVPerp);

		Matrix_TransformVector(&iA, &lA);

		//Step b: Determine if the magnitude of the angular impulse / momentum overcomes the magnitude of static friction
		if(Vector_GetMag(&lA) <= staticMag)
		{
			//printf("Cancelling1\n");
			//If it does not, apply an opposing impulse (Note that lA is already in the direction opposing A's angular velocity)
			RigidBody_ApplyInstantaneousTorque(body1, &lA);
		}
		else
		{
			//If it does, apply an impulse in the direction opposing intending movement equal to dynamicMag in magnitude
			float direction = Vector_DotProduct(body1->angularVelocity, &lA);
			if(direction < -FLT_EPSILON) direction = 1.0f;
			else if(direction > FLT_EPSILON) direction = -1.0f;
			else direction = 0.0f;
	
			Vector_Normalize(&lA);
			Vector_Scale(&lA, dynamicMag * direction);
			RigidBody_ApplyInstantaneousTorque(body1, &lA);
		}
	
	}
	
	//Object 2
	if(body2 != NULL && body2->inverseMass != 0.0f && !body2->freezeRotation)
	{
		//Step a: Compute the angular impulse / momentum in the direction of surface normal
		Vector lB;
		Vector_INIT_ON_STACK(lB, 3);
		
		Matrix iB;
		Matrix_INIT_ON_STACK(iB, 3, 3);
		RigidBody_CalculateMomentOfInertiaInWorldSpace(&iB, body2);

		Vector_GetScalarProduct(&lB, collision->minimumTranslationVector, relAVPerp);
		Matrix_TransformVector(&iB, &lB);

		//Step b: Determine if the magnitude of the angular impulse / momentum overcomes the magnitude of static friction
		if(Vector_GetMag(&lB) <= staticMag)
		{
			//If it does not, apply an opposing impulse (Note that lA is in the direction of B's angular velocity)
			Vector_Scale(&lB, -1.0f);
			RigidBody_ApplyInstantaneousTorque(body2, &lB);
		}
		else
		{


			//If it does, apply an impulse in the direction opposing intending movement equal to dynamicMag in magnitude
			float direction = Vector_DotProduct(body2->angularVelocity, &lB);
			if(direction < -FLT_EPSILON) direction = 1.0f;
			else if(direction > FLT_EPSILON) direction = -1.0f;
			else direction = 0.0f;

			Vector_Normalize(&lB);
			Vector_Scale(&lB, dynamicMag * direction);
			RigidBody_ApplyInstantaneousTorque(body2, &lB);
		}
	}
}


///
//Calculates and applies the rolling resistance between two objects
//
//Parameters:
//	collision: A pointer to the collision on which to compute rolling resistances
//	pointsOfCollision: An array of pointers to vectors containing the points of collision in worldspace for each object
//	resistanceCoefficient: The net rolling resistance coefficient between the two objects
static void PhysicsManager_ApplyRollingResistance(struct Collision* collision, Vector** pointsOfCollision, const float resistanceCoefficient)
{
	//Step 1: Compute the magnitude of the rolling resistance force
	float resistanceMag = collision->resolutionImpulse * resistanceCoefficient;

	//Step 2: Compute the radii from the center of mass to the point of collision for each object
	Vector radius1;
	Vector radius2;

	Vector_INIT_ON_STACK(radius1, 3);
	Vector_INIT_ON_STACK(radius2, 3);
	
	Vector_Subtract(&radius1, pointsOfCollision[0], collision->obj1Frame->position);
	Vector_Subtract(&radius2, pointsOfCollision[1], collision->obj2Frame->position);

	//Step 3: Compute the maximum angular impulse for each object due to rolling resistance
	float angularResistance1 = resistanceMag * Vector_GetMag(&radius1);
	float angularResistance2 = resistanceMag * Vector_GetMag(&radius2);

	
	//Step 4: Calculate the current angular momentum
	Vector angularMomentum1;
	Vector angularMomentum2;

	Vector_INIT_ON_STACK(angularMomentum1, 3);
	Vector_INIT_ON_STACK(angularMomentum2, 3);

	if(collision->obj1->body != NULL)
		RigidBody_CalculateAngularMomentum(&angularMomentum1, collision->obj1->body);
	if(collision->obj2->body != NULL)
		RigidBody_CalculateAngularMomentum(&angularMomentum2, collision->obj2->body);

	//Include the angular impulse which is about to be applied!
	if(collision->obj1->body != NULL)
		Vector_Increment(&angularMomentum1, collision->obj1->body->netInstantaneousTorque);
	if(collision->obj2->body != NULL)
		Vector_Increment(&angularMomentum2, collision->obj2->body->netInstantaneousTorque);

	//Step 5: Get the magnitudes of the momentum along the collision plane
	Vector proj;
	Vector_INIT_ON_STACK(proj, 3);

	Vector_GetProjection(&proj, &angularMomentum1, collision->minimumTranslationVector);
	Vector_Decrement(&angularMomentum1, &proj);
	

	Vector_GetProjection(&proj, &angularMomentum2, collision->minimumTranslationVector);
	Vector_Decrement(&angularMomentum2, &proj);

	float mag1 = Vector_GetMag(&angularMomentum1);
	float mag2 = Vector_GetMag(&angularMomentum2);

	//Step 6: Limit the magnitude of rolling resistance by the magnitude of angular momenum along collision plane
	if(angularResistance1 > mag1) angularResistance1 = mag1;
	if(angularResistance2 > mag2) angularResistance2 = mag2;

	//Step 7: Create rolling resistance angular impulses
	Vector resistance1;
	Vector resistance2;
	Vector_INIT_ON_STACK(resistance1, 3);
	Vector_INIT_ON_STACK(resistance2, 3);

	Vector_Normalize(&angularMomentum1);
	Vector_Normalize(&angularMomentum2);

	Vector_GetScalarProduct(&resistance1, &angularMomentum1, -angularResistance1);
	Vector_GetScalarProduct(&resistance2, &angularMomentum2, -angularResistance2);

	//Step 8: apply the rolling resistance
	if(collision->obj1->body != NULL)
		RigidBody_ApplyInstantaneousTorque(collision->obj1->body, &resistance1);
	if(collision->obj2->body != NULL)
		RigidBody_ApplyInstantaneousTorque(collision->obj2->body, &resistance2);

}
