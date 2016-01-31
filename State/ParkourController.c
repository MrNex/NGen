#if defined __linux__

//Enable POSIX definitions for timespec
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <time.h>

#endif

#include <float.h>
#include <math.h>

#include "ParkourController.h"

#include "../Manager/CollisionManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Data/LinkedList.h"

#include "Remove.h"
#include "ColorCopy.h"

struct State_ParkourController_Members
{
	float acceleration;
	float maxVelocity;
	float angularVelocity;
	float jumpMag;

	unsigned char verticalRunning;	//Is this state currently vertically running up a wall?
	unsigned char horizontalRunning;//Is this state currently horizontally running along a wall?

	float spinningTime;
	float spinningTimer;

	float shootCooldown;
	float shootTimer;

	float reactionTime;
	float reactionTimer;
	unsigned char reactionLock;

	unsigned char charging;
	unsigned char chargeAvailable;

	unsigned char wasOnGround;

	float rollTime;
	float rollTimer;

	float skewTime;
	float skewTimer;
	float restoreTime;
	float restoreTimer;

	Vector* wallNormal;
};

///
//Initializes a Template state
//
//Parameters:
//	s: A pointer to the state to initialize
//	acceleration: The acceleration of the parkourController
//	maxVelocity: The maximum linear velocity of the parkourController
//	angularVelocity: The angular velocity of the parkour controller (How fast can you turn?)
//	jumpImpulse: The force exerted when jumping
//	shootSpeed: The amount of seconds you must wait before shooting againS
void State_ParkourController_Initialize(State* s, const float acceleration, const float maxVelocity, const float angularVelocity, const float jumpImpulse, const float shootSpeed)
{
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)malloc(sizeof(struct State_ParkourController_Members));
	s->members = (State_Members*)members;

	//Set state members
	members->acceleration = acceleration;
	members->angularVelocity = angularVelocity;
	members->maxVelocity = maxVelocity;
	members->angularVelocity = angularVelocity;
	members->jumpMag = jumpImpulse;

	members->verticalRunning = 0;
	members->horizontalRunning = 0;

	members->spinningTime = 3.14159f / spinningRate;
	members->spinningTimer = members->spinningTime;

	members->rollTime = 2.0f * 3.14159 / rollRate;
	members->rollTimer = members->rollTime;

	members->skewTime = 2.0f;
	members->skewTimer = members->skewTime;

	members->restoreTime = 0.2f;
	members->restoreTimer = members->restoreTime;

	members->shootTimer = 0.0f;
	members->shootCooldown = shootSpeed;	

	members->reactionTime = 0.2f;
	members->reactionTimer = 0.0f;
	members->reactionLock = 0;

	members->charging = 0;
	members->chargeAvailable = 1;

	members->wasOnGround = 0;

	members->wallNormal = Vector_Allocate();
	Vector_Initialize(members->wallNormal, 3);

	//Set functions
	s->State_Update = State_ParkourController_Update;
	s->State_Members_Free = State_ParkourController_Free;
}

///
//Frees memory allocated by a Template state
//
//Parameters:
//	state: The Template state to free
void State_ParkourController_Free(State* state)
{
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;
	Vector_Free(members->wallNormal);
	free(members);	
}

///
//Determines if the ParkourController object is currently on the ground.
//
//Parameters:
//	obj: A pointer to the Object attached to the ParkourController state
//	state: A pointer to the ParkourController state
//
//Returns:
//	0 if object is not on the ground, 1 if it is
static unsigned char State_ParkourController_IsOnGround(GObject* obj)
{

	//Loop through the current list of collisions
	struct LinkedList_Node* current = obj->collider->currentCollisions->head;
	struct Collision* currentCollision = NULL;

	while(current!=NULL)
	{
		//Get the next collision
		currentCollision = (struct Collision*)current->data;
		
		//Determine if this obj is obj1 in collision or obj2
		//This is important because the surface normal always points toward obj1!
		if(obj == currentCollision->obj1)
		{
			//If Obj is on ground, MTV would point in direction of positive Y axis
			if(currentCollision->minimumTranslationVector->components[1] == 1.0f)
			{
				return 1;
			}
		}
		else
		{
			//If Obj is on ground, MTV would point in direction of negative Y axis
			if(currentCollision->minimumTranslationVector->components[1] == -1.0f)
			{
				return 1;
			}
		}

		current = current->next;
	}

	return 0;
}

///
//Accelerates a gameobject to the degree defined by the state
//
//PArameters:
//	obj: The gameobject to accelerate
//	state: The ParkourController state attached to the game object
static void State_ParkourController_Accelerate(GObject* obj, State* state)
{
	//Grab the state members
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//Grab the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	//Determine the direction of acceleration
	Vector netForce;
	Vector_INIT_ON_STACK(netForce, 3);
	Vector direction;
	Vector_INIT_ON_STACK(direction, 3);
	if(InputManager_IsKeyDown('w'))
	{
		//Get forward vector of camera
		Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
		//Project onto XY Plane
		Vector perp;
		Vector_INIT_ON_STACK(perp, 3);

		Vector_GetProjection(&perp, &direction, &Vector_E2);
		Vector_Decrement(&direction, &perp);

		//Add vector to netForce
		//Since this is the cameras "forward vector" we must add the negative of it.
		//BEcause forward is the negative Z axis
		Vector_Decrement(&netForce, &direction);
	}
	if(InputManager_IsKeyDown('s'))
	{
		//Get back vector of camera
		//Get forward vector of camera
		Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
		//Project onto XY Plane
		Vector perp;
		Vector_INIT_ON_STACK(perp, 3);

		Vector_GetProjection(&perp, &direction, &Vector_E2);
		Vector_Decrement(&direction, &perp);

		//Add vector to netForce
		//Since this is the cameras "forward vector" we must add the negative of it.
		//BEcause forward is the negative Z axis
		Vector_Increment(&netForce, &direction);
	}
	if(InputManager_IsKeyDown('d'))
	{
		//Get forward vector of camera
		Matrix_SliceRow(&direction, cam->rotationMatrix, 0, 0, 3);
		//Project onto XY Plane
		Vector perp;
		Vector_INIT_ON_STACK(perp, 3);

		Vector_GetProjection(&perp, &direction, &Vector_E2);
		Vector_Decrement(&direction, &perp);

		//Add vector to netForce
		//Since this is the cameras "forward vector" we must add the negative of it.
		//BEcause forward is the negative Z axis
		Vector_Increment(&netForce, &direction);
	}
	if(InputManager_IsKeyDown('a'))
	{
		//Get forward vector of camera
		Matrix_SliceRow(&direction, cam->rotationMatrix, 0, 0, 3);
		//Project onto XY Plane
		Vector perp;
		Vector_INIT_ON_STACK(perp, 3);

		Vector_GetProjection(&perp, &direction, &Vector_E2);
		Vector_Decrement(&direction, &perp);

		//Add vector to netForce
		//Since this is the cameras "forward vector" we must add the negative of it.
		//BEcause forward is the negative Z axis
		Vector_Decrement(&netForce, &direction);
	}

	//Scale netforce to the acceleration magnitude
	Vector_Normalize(&netForce);
	Vector_Scale(&netForce, members->acceleration);

	//Only apply impulse if velocity is less than max speed
	if(Vector_GetMag(obj->body->velocity) < members->maxVelocity)
	{
		//Apply the impulse
		RigidBody_ApplyForce(obj->body, &netForce, &Vector_ZERO);
	}
	else
	{
		//Limit velocity
		Vector_Normalize(obj->body->velocity);
		Vector_Scale(obj->body->velocity, members->maxVelocity);
	}
}

static void State_ParkourController_Rotate(State* state)
{
	//Get a reference to the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	//Grab the state members
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//If the player's mouse is locked
	if(InputManager_GetInputBuffer().mouseLock)
	{
		//Get the change in mouse position
		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		//If there is X rotation
		if(deltaMouseX != 0)
		{
			//Rotate the camera
			Camera_ChangeYaw(cam, (float)deltaMouseX * members->angularVelocity);
		}
		//If there is Y rotation
		if(deltaMouseY != 0)
		{

			//TODO: Prevent camera from looking "Too high"
			//Rotate the camera
			Camera_ChangePitch(cam, (float)deltaMouseY * members->angularVelocity);
		}
	}
}

///
//Allows the parkour controller to jump
//
//Parameters:
//	obj: A pointer to the object attached to the ParkourController state
//	state: The parkour state updating the object
static void State_ParkourController_Jump(GObject* obj, State* state)
{
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;
	
	Vector jumpImpulse;
	Vector_INIT_ON_STACK(jumpImpulse, 3);
	
	jumpImpulse.components[1] = members->jumpMag;
	RigidBody_ApplyImpulse(obj->body, &jumpImpulse, &Vector_E1);
}

///
//Allows the parkour controller to horizontally wallrun
//
//Parameters:
//	obj: A pointer to the object attached to the parkourController state
//	state: The parkourController state updating the object
static void State_ParkourController_HorizontalWallrun(GObject* obj, State* state)
{
	//printf("Horizontal\n");

	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//Zero downward velocity
	if(obj->body->velocity->components[1] < 0.0f)
	{
		Vector impulse;
		Vector_INIT_ON_STACK(impulse, 3);
		impulse.components[1] = -obj->body->velocity->components[1] / obj->body->inverseMass;
		RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);
	}
	
	//Accelerate along wall
	//Get the projection of the forward vector onto the wall's plane
	
	//Start by getting for forward vector of the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	Vector forward;
	Vector_INIT_ON_STACK(forward, 3);

	Matrix_SliceRow(&forward, cam->rotationMatrix, 2, 0, 3);
	//Forward of camera is back of object...
	Vector_Scale(&forward, -1.0f);

	//Project the forward vector onto the wall plane
	Vector perp;
	Vector_INIT_ON_STACK(perp, 3);

	Vector_GetProjection(&perp, &forward, members->wallNormal);
	Vector_Decrement(&forward, &perp);

	//Set the Y component to 0 to get horizontal vector along wall!
	forward.components[1] = 0.0f;

	Vector_Normalize(&forward);

	//MAke sure the velocity in this direction is not too much
	float magVelAlongWall = Vector_DotProduct(&forward, obj->body->velocity);

	if(magVelAlongWall < members->maxVelocity)
	{
		Vector_Scale(&forward, members->maxVelocity);

		RigidBody_ApplyImpulse(obj->body, &forward, &Vector_ZERO);

	}
	else
	{
		Vector_Normalize(&forward);
		Vector_GetScalarProduct(obj->body->velocity, &forward, members->maxVelocity);
	}

	//Apply a cohesive force to the wall to make sure you do not fall off
	float mag = Vector_DotProduct(obj->body->velocity, members->wallNormal);

	Vector cohesive;
	Vector_INIT_ON_STACK(cohesive, 3);

	if(mag > FLT_EPSILON)
	{
		Vector_GetScalarProduct(&cohesive, members->wallNormal, -mag);
	
	}
	else if(mag < FLT_EPSILON)
	{
		Vector_GetScalarProduct(&cohesive, members->wallNormal, mag);
	}
	RigidBody_ApplyImpulse(obj->body, &cohesive, members->wallNormal);
	

}

///
//Allows the parkour controller to vertically wallrun
//
//Parameters:
//	obj: A pointer to the object attached to the parkourController state
//	state: The parkourController state updating the object
static void State_ParkourController_VerticalWallrun(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;


	Vector impulse;
	Vector_INIT_ON_STACK(impulse, 3);
	impulse.components[1] = members->jumpMag;
	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);

	//Cap max upward speed
	if(obj->body->velocity->components[1] > members->maxVelocity)
	{
		obj->body->velocity->components[1] = members->maxVelocity;
	}

	if(obj->body->velocity->components[0] != 0.0f || obj->body->velocity->components[2])
	{
		Vector_Copy(&impulse, &Vector_ZERO);

		//impulse.components[0] -= obj->body->velocity->components[0];
		//impulse.components[2] -= obj->body->velocity->components[2];
		impulse.components[0] -= members->maxVelocity * members->wallNormal->components[0];
		impulse.components[2] -= members->maxVelocity * members->wallNormal->components[2];

		RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);
	}
}

///
//Allows the parkour controller to run up a wall
//
//Parameters:
//	obj: A pointer to the object attached to the parkourController state
//	state: The parkourController state updating the object
static void State_ParkourController_Wallrun(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//If we are not vertical wallrunning yet
	if(members->verticalRunning == 0 && members->horizontalRunning == 0)
	{
		//Loop through the list of collisions this object was involved in
		struct LinkedList_Node* currentNode = obj->collider->currentCollisions->head;
		while(currentNode != NULL)
		{
			//Get the current collision
			struct Collision* current = (struct Collision*)currentNode->data;
		
			//Make sure this collision is with a wall
			//TODO: Epsilon check!!!
			if(current->minimumTranslationVector->components[0] != 0.0f || current->minimumTranslationVector->components[2] != 0.0f)
			{
				//Make a copy of the collision normal in case of manipulation
				Vector currentNormal;
				Vector_INIT_ON_STACK(currentNormal, 3);
				Vector_Copy(&currentNormal, current->minimumTranslationVector);

				//Make sure the normal is pointing toward this object
				if(current->obj1 != obj)
				{
					Vector_Scale(&currentNormal, -1.0f);
				}


				//Next we must determine what kind of wallrun is happening
				//Start by getting for forward vector of the camera
				Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

				Vector forward;
				Vector_INIT_ON_STACK(forward, 3);

				Matrix_SliceRow(&forward, cam->rotationMatrix, 2, 0, 3);

				//Project the forward vector onto the XY plane
				Vector perp;
				Vector_INIT_ON_STACK(perp, 3);

				Vector_GetProjection(&perp, &forward, &Vector_E2);
				Vector_Decrement(&forward, &perp);

				Vector_Normalize(&forward);

				//Get the dot product of the forward vector and collision normal
				float dotProduct = Vector_DotProduct(&forward, &currentNormal);

				//If the dot product is closer to 1, we are starting to vertically wallrun
				if(dotProduct > 0.75f)
				{
					members->verticalRunning = 1;
					//Vertical wall running always has higher precedence than horizontal wall running
					members->horizontalRunning = 0;
					//SEt the wall normal of state
					Vector_Copy(members->wallNormal, &currentNormal);
					members->reactionTimer = 0.0f;
					members->chargeAvailable = 1;
					
					DirectionalLight* d = RenderingManager_GetRenderingBuffer()->directionalLight;

					float yaw = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
					float pitch = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		
					d->color->components[0] = cosf(yaw) * sinf(yaw) + .9f;
					d->color->components[1] = cosf(yaw) * sinf(pitch) + .9f;
					d->color->components[2] = cosf(pitch) + .9;

					break;
				}
				else if(dotProduct > 0.0f)
				{
					members->horizontalRunning = 1;
					//Set the wall normal of state
					Vector_Copy(members->wallNormal, &currentNormal);
					members->reactionTimer = 0.0f;
					members->chargeAvailable = 1;

					DirectionalLight* d = RenderingManager_GetRenderingBuffer()->directionalLight;

					float yaw = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
					float pitch = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		
					d->color->components[0] = cosf(yaw) * sinf(yaw) + .9f;
					d->color->components[1] = cosf(yaw) * sinf(pitch) + .9f;
					d->color->components[2] = cosf(pitch) + .9;


				}
			}
			currentNode = currentNode->next;
		}
	}

	//If we are vertical wall running
	if(members->verticalRunning)
	{

		State_ParkourController_VerticalWallrun(obj, state);

	}
	//else If we are horizontal wall running
	else if(members->horizontalRunning)
	{	
		State_ParkourController_HorizontalWallrun(obj, state);
	}
}

///
//Lets the controller jump off of a wall
//Parameters:
//	obj: A pointer to The object attached to the ParkourController state
//	state: A pointer to the ParkourController state updating the object
void State_ParkourController_WallJump(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	Vector impulse;
	Vector_INIT_ON_STACK(impulse, 3);
	Vector_Copy(&impulse, members->wallNormal);
	Vector_Scale(&impulse, members->maxVelocity / obj->body->inverseMass);

	if(members->horizontalRunning)
		Vector_Scale(&impulse, 0.5f);

	impulse.components[1] = -obj->body->velocity->components[1] /obj->body->inverseMass + members->jumpMag;


	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);
	
	if(members->horizontalRunning)
	{
		Vector_Copy(&impulse, &Vector_E2);
		Vector_Scale(&impulse, members->jumpMag);
		RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);
	}
	
}

///
//Lets the ParkourController vault over a wall
//
//Parameters:
//	obj: A pointer to the object attached to the ParkourController state
//	state: 
void State_ParkourController_WallVault(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;
	
	Vector impulse;
	Vector_INIT_ON_STACK(impulse, 3);

	Vector_Copy(&impulse, members->wallNormal);
	Vector_Scale(&impulse, -members->maxVelocity / obj->body->inverseMass);
	impulse.components[1] = -obj->body->velocity->components[1]/obj->body->inverseMass + members->jumpMag;
	
	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);

	//Provide time for player to let go of 'r'
	members->reactionTimer = 0.0f;
	members->reactionLock = 1;

}	

void State_ParkourController_Shoot(GObject* obj, State* state)
{
	//Get the members of the state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//Get a reference to the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	
	if(InputManager_GetInputBuffer().mouseLock)
	{
		//IF we can shoot again
		if(members->shootTimer >= members->shootCooldown)
		{
			//Get the forward vector of the camera
			Vector direction;
			Vector_INIT_ON_STACK(direction, 3);

			Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
			Vector_Scale(&direction, -1.0f);

			//Create the bullet object
			GObject* bullet = GObject_Allocate();
			GObject_Initialize(bullet);

			//Set the appearance
			bullet->mesh = AssetManager_LookupMesh("Cube");
			//bullet->texture = AssetManager_LookupTexture("White");
			bullet->material = Material_Allocate();
			Material_Initialize(bullet->material, AssetManager_LookupTexture("Jacob"));
			//*Matrix_Index(bullet->material->colorMatrix, 1, 1) = 0.0f;
			//*Matrix_Index(bullet->material->colorMatrix, 2, 2) = 0.0f;

			//Create ridgid body
			bullet->body = RigidBody_Allocate();
			RigidBody_Initialize(bullet->body, bullet->frameOfReference, 1.0f);
			bullet->body->coefficientOfRestitution = 0.2f;
			bullet->body->rollingResistance = 0.2f;	
			bullet->body->staticFriction = 0.4f;
			bullet->body->dynamicFriction = 0.2f;	
	
			//Create collider
			bullet->collider = Collider_Allocate();
			ConvexHullCollider_Initialize(bullet->collider);
			ConvexHullCollider_MakeRectangularCollider(bullet->collider->data->convexHullData, 2.0f, 2.0f, 2.0f);

			//Position bullet
			Vector transform;
			Vector_INIT_ON_STACK(transform, 3);

			Vector_GetScalarProduct(&transform, &direction, 2.8243f);
			Vector_Increment(&transform, obj->frameOfReference->position);
			GObject_Translate(bullet, &transform);

			Vector_Copy(&transform, &Vector_ZERO);
			transform.components[2] = 1.0f;
			GObject_Rotate(bullet, &transform, 3.14159f);

			//Scale bullet
			Vector_Copy(&transform, &Vector_ZERO);
			transform.components[0] = transform.components[1] = transform.components[2] = 0.5f;
			GObject_Scale(bullet, &transform);

			//Apply impulse
			Vector_Scale(&direction, 25.0f);
			RigidBody_ApplyImpulse(bullet->body, &direction, &Vector_ZERO);

			//Add the remove state
			State* state = State_Allocate();
			State_Remove_Initialize(state, 7.0f);
			GObject_AddState(bullet, state);

			//Add the bullet to the world
			ObjectManager_AddObject(bullet);

			//Set shoot timer to 0
			members->shootTimer = 0.0f;
		}
	}	
}

void State_ParkourController_Charge(GObject* obj, struct State_ParkourController_Members* members)
{
	Vector_Copy(obj->body->velocity, &Vector_ZERO);

	if(!InputManager_IsMouseButtonPressed(2))
	{
		Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

		//Get the forward vector of the camera
		Vector direction;
		Vector_INIT_ON_STACK(direction, 3);

		Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
		Vector_Scale(&direction, -1.0f);
		
		Vector_Scale(&direction, members->maxVelocity / obj->body->inverseMass);

		RigidBody_ApplyImpulse(obj->body, &direction, &Vector_ZERO);

		members->charging = 0;
		members->rollTimer = 0.0f;
		members->skewTimer = members->skewTime;
		members->restoreTimer = 0.0f;
	}
}

void State_ParkourController_Roll(GObject* obj, struct State_ParkourController_Members* members)
{
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	Vector direction;
	Vector_INIT_ON_STACK(direction, 3);

	//Get forward vector of camera
	Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
	//Project onto XY Plane
	Vector perp;
	Vector_INIT_ON_STACK(perp, 3);

	Vector_GetProjection(&perp, &direction, &Vector_E2);
	Vector_Decrement(&direction, &perp);

	Vector_Normalize(&direction);
	Vector_Scale(&direction, -members->maxVelocity / obj->body->inverseMass);

	RigidBody_ApplyImpulse(obj->body, &direction, &Vector_ZERO);

	DirectionalLight* d = RenderingManager_GetRenderingBuffer()->directionalLight;

	float yaw = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
	float pitch = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		
	d->color->components[0] = cosf(yaw) * sinf(yaw) + .8f;
	d->color->components[1] = cosf(yaw) * sinf(pitch) + .8f;
	d->color->components[2] = cosf(pitch) + .8;


}

///
//Updates a GObject using a Template state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_ParkourController_Update(GObject* obj, State* state)
{
	//Get a reference to the state members
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;


	//If it needs to be updated, update the shoot timer
	if(members->shootTimer < members->shootCooldown)
	{
		float dt = TimeManager_GetDeltaSec();
		members->shootTimer += dt;
	}	
	//If the left mouse button is down, shoot
	/*
	   //For now left mouse button performs wallruns instead
	if(InputManager_IsMouseButtonPressed(0))
	{
		State_ParkourController_Shoot(obj, state);
	}
	*/

	//Get a reference to the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	//If a wall vault was performed, r must be released before
	//the reaction timer will start again
	if(members->reactionLock && !InputManager_IsMouseButtonPressed(0))
	{
		members->reactionLock = 0;
	}

	//As soon as r is pressed, begin counting and do not stop until
	//A successful wallrun or the ground is touched
	char reacted = 0;
	if((InputManager_IsMouseButtonPressed(0) || members->reactionTimer > 0.0f) && ! members->reactionLock)
	{
		reacted = 1;

		//If not yet running, reduce reaction timer	
		if(!members->verticalRunning && ! members->horizontalRunning)
		{
			members->reactionTimer += TimeManager_GetDeltaSec();	
		}
	}


	if(members->charging)
	{
		State_ParkourController_Charge(obj, members);		
	}
	//Determine if the object is colliding with anything
	else if(obj->collider->currentCollisions->size > 0)
	{

		//Determine if the object is on the ground
		if(State_ParkourController_IsOnGround(obj))
		{
			if(reacted && !members->wasOnGround)
			{
				if(members->reactionTimer <= members->reactionTime && reacted)
				{
					State_ParkourController_Roll(obj, members);
				}
			}

			members->wasOnGround = 1;
			members->chargeAvailable = 1;

			members->reactionTimer = 0.0f;

			//Allow the object to accelerate
			State_ParkourController_Accelerate(obj, state);	
			if(InputManager_IsKeyDown(' '))
			{
				State_ParkourController_Jump(obj, state);
			}
		}
		else
		{
			members->wasOnGround = 0;
		}

		//If we have already reacted even if the object is not on the floor
		if(reacted)
		{
			if(members->reactionTimer <= members->reactionTime || members->verticalRunning || members->horizontalRunning)
			{
				printf("Reaction timer:\t%f\n", members->reactionTimer);

				//Attempt to wallrun!
				State_ParkourController_Wallrun(obj, state);
			}
		}
		else if(members->verticalRunning || members->horizontalRunning)
		{
			//Wall jump!
			State_ParkourController_WallJump(obj, state);

			//If vertically wallrunning, start spinning right round baby right round!
			if(members->verticalRunning) members->spinningTimer = 0.0f;	
	
			//The object was wallrunning but now (s)he stopped before the wall ended
			members->verticalRunning = members->horizontalRunning = 0;
			//Do not start the reaction timer again until
			//User lets go of r
			members->reactionTimer = 0.0f;
			members->reactionLock = 1;
		}
	}
	else if(members->verticalRunning || members->horizontalRunning)
	{
		if(members->verticalRunning)
		{
			State_ParkourController_WallVault(obj, state);
		}
		
		members->reactionLock = 1;
		members->reactionTimer = 0.0f;

		//The character was wallrunning but now there is no wall
		members->verticalRunning = members->horizontalRunning = 0;
	}
	else if(members->chargeAvailable)		//Not touching anything
	{
		if(InputManager_IsMouseButtonPressed(2))
		{
			members->charging = 1;
			members->chargeAvailable = 0;
			members->skewTimer = 0.0f;
		}
	}

	if(members->skewTimer < members->skewTime)
	{
		float dt = TimeManager_GetDeltaSec();
		members->skewTimer += dt;
		cam->nearPlane -= skewRate * dt;


		DirectionalLight* d = RenderingManager_GetRenderingBuffer()->directionalLight;

		static float toggle = -1.0f;

		static int index = 0;
		d->color->components[index] += dt * toggle;
		if(d->color->components[index] > 1.0f)
		{	
			d->color->components[index] = 1.0f;
			toggle *= -1.0f;

			index = EnvironmentManager_Rand(0, 3);
		}
		else if(d->color->components[index] < 0.0f)
		{
			d->color->components[index] = 0.0f;
			toggle *= -1.0f;
			index = EnvironmentManager_Rand(0, 3);
		}


		Camera_RefreshPerspectiveMatrix(cam);
	}
	if(members->restoreTimer < members->restoreTime)
	{
		float dt = TimeManager_GetDeltaSec();
		members->restoreTimer += dt;
		cam->nearPlane += restoreRate * dt;
		if(cam->nearPlane >= 1.0f)
		{
			cam->nearPlane = 1.0f;
			members->restoreTimer = members->restoreTime;
		}

		Camera_RefreshPerspectiveMatrix(cam);
	}

	//If the parkour controller currently spinning off a wall or rolling from a charge, let it continue, else let the player rotate the controller
	unsigned char busy = 0;
	if(members->rollTimer < members->rollTime)
	{
		float dt = TimeManager_GetDeltaSec();
		members->rollTimer += dt;
		
		//Get camera's forward vector
		Vector forward;
		Vector_INIT_ON_STACK(forward, 3);
		Matrix_SliceRow(&forward, cam->rotationMatrix, 2, 0, 3);
		Vector_Scale(&forward, -1.0f);

		Vector_Copy(&forward, &Vector_E3);

		Camera_Rotate(cam, &forward, rollRate * dt);
		
		busy = 1;

	}
	if(members->spinningTimer < members->spinningTime)
	{
		float dt = TimeManager_GetDeltaSec();
		members->spinningTimer += dt;
		Camera_ChangeYaw(cam, spinningRate * dt);

		busy = 1;
	}
	if(!busy)
	{
		//Allow the user to rotate the parkour controller
		State_ParkourController_Rotate(state);
	}

	//Set the position of the camera to match the object
	Camera_SetPosition(cam, obj->frameOfReference->position);
}




