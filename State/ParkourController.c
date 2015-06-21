#if defined __linux__

//Enable POSIX definitions for timespec
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <time.h>

#endif

#include "ParkourController.h"

#include "../Manager/CollisionManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/TimeManager.h"

#include "../Data/LinkedList.h"


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
void State_ParkourController_Initialize(State* s, const float acceleration, const float maxVelocity, const float angularVelocity, const float jumpImpulse)
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
	if(InputManager_IsKeyDown('W'))
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
	if(InputManager_IsKeyDown('S'))
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
	if(InputManager_IsKeyDown('D'))
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
	if(InputManager_IsKeyDown('A'))
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
		RigidBody_ApplyImpulse(obj->body, &netForce, &Vector_ZERO);
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

		//Get the change in time!
		float dt = TimeManager_GetDeltaSec();

		//If there is X rotation
		if(deltaMouseX != 0)
		{
			//Rotate the camera
			Camera_ChangeYaw(cam, dt * (float)deltaMouseX * members->angularVelocity);
		}
		//If there is Y rotation
		if(deltaMouseY != 0)
		{

			//TODO: Prevent camera from looking "Too high"
			//Rotate the camera
			Camera_ChangePitch(cam, dt * (float)deltaMouseY * members->angularVelocity);
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

static void State_ParkourController_Wallrun(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_ParkourController_Members* members = (struct State_ParkourController_Members*)state->members;

	//If we are not wallrunning yet
	if(members->horizontalRunning == 0 && members->verticalRunning == 0)
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
		
			}
			currentNode = currentNode->next;
		}
	}
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
	
	//Get a reference to the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	//Determine if the object is colliding with anything
	if(obj->collider->currentCollisions->size > 0)
	{
		//Determine if the object is on the ground
		if(State_ParkourController_IsOnGround(obj))
		{
			//Allow the object to accelerate
			State_ParkourController_Accelerate(obj, state);	
			if(InputManager_IsKeyDown(' '))
			{
				State_ParkourController_Jump(obj, state);
			}
		}

		//If the spacebar is being pressed even if the object is not on the floor
		if(InputManager_IsKeyDown(' '))
		{
			//Attempt to wallrun!
			State_ParkourController_Wallrun(obj, state);
		}
	}
	
	//If the parkour controller currently spinning off a wall, let it continue, else let the player rotate the controller
	if(members->spinningTimer < members->spinningTime)
	{

	}
	else
	{
		//Allow the user to rotate the parkour controller
		State_ParkourController_Rotate(state);
	}

	//Set the position of the camera to match the object
	Camera_SetPosition(cam, obj->frameOfReference->position);
}




