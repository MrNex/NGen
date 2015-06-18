#if defined __linux__

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif

#include "RunnerController.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "../Manager/RenderingManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/TimeManager.h"

#include "../Data/LinkedList.h"

struct State_RunnerController_Members
{
	float acceleration;
	float maxVelocity;
	float angularVelocity;
	float jumpMag;

	unsigned char verticalRunning;
	unsigned char horizontalRunning;

	float spinningTime;
	float spinningTimer;

	Vector* wallNormal;
};

///
//Initializes a runner controller state
//
//Parameters:
//	state: A pointer to the state to initialize as a runner controller
//	acceleration: The acceleration of the runner
//	maxvelocity: The max speed of the runner
//	angularVelocity: the angular velocity of the runner
//	jumpMag: The magnitude of the impulse which will be applied when the player jumps
void State_RunnerController_Initialize(State* state, const float acceleration, const float maxVelocity, const float angularVelocity, const float jumpMag)
{
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)malloc(sizeof(struct State_RunnerController_Members));
	state->members = (State_Members)members;

	members->acceleration = acceleration;
	members->angularVelocity = angularVelocity;
	members->maxVelocity = maxVelocity;
	members->jumpMag = jumpMag;

	members->verticalRunning = 0;
	members->horizontalRunning = 0;

	members->spinningTime = 3.14159f / spinningRate;
	members->spinningTimer = members->spinningTime;

	members->wallNormal = Vector_Allocate();
	Vector_Initialize(members->wallNormal, 3);

	state->State_Members_Free = State_RunnerController_Free;
	state->State_Update = State_RunnerController_Update;
}

///
//Frees members in a runner controller state
//
//Parameters:
//	state: A pointer to the state to free the members of
void State_RunnerController_Free(State* state)
{
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;
	Vector_Free(members->wallNormal);
	free(members);
}


static unsigned char State_RunnerController_IsOnGround(GObject* obj, State* state)
{
	//State is unused
	(void)state;

	//Check if the object is on the ground...
	unsigned char onGround = 0;

	//Loop through all current collisions
	struct LinkedList_Node* current = obj->collider->currentCollisions->head;
	struct Collision* currentCollision = NULL;
	while(current != NULL)
	{
		currentCollision = (struct Collision*)current->data;
		//Determine if this obj is obj1 or obj2
		if(obj == currentCollision->obj1)
		{
			//Make sure collision normal is pointing up
			if(currentCollision->minimumTranslationVector->components[1] == 1.0f)
			{
				onGround = 1;
				break;
			}
		}
		else
		{
			//Make sure collision normal is pointing down
			if(currentCollision->minimumTranslationVector->components[1] == -1.0f)
			{
				onGround = 1;
				break;
			}
		}

		current = current->next;
	}

	return onGround;
}

///
//Updates the object attached to a runner controller
//This object will always accelerate forward while on a surface unless the velocity is already at the max velocity.
//
//Parameters:
//	obj: A pointer to the gameobject to update as a runner
//	state: A pointer to the runner controller state
void State_RunnerController_Update(GObject* obj, State* state)
{
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;

	//If the object is colliding with something, allow it to possibly wall run
	if(obj->collider->currentCollisions->size > 0)
	{
		//If the user is pressing RMB
		if(InputManager_IsMouseButtonPressed(2))
		{
			State_RunnerController_Wallrun(obj, state);
		}
		else if(members->verticalRunning || members->horizontalRunning)
		{
			//Wall jump
			if(members->verticalRunning)
			{
				members->spinningTimer = 0.0f;
			}

			State_RunnerController_WallJump(obj, state);

			members->verticalRunning = members->horizontalRunning = 0;

		}
		else
		{
			if(State_RunnerController_IsOnGround(obj, state))
			{
				State_RunnerController_Accelerate(obj, state);
			}
		}


		//If the user is pressing LMB
		if(InputManager_IsMouseButtonPressed(0))
		{
			State_RunnerController_Jump(obj, state);
		}

	}
	else if(members->verticalRunning || members->horizontalRunning)
	{

		//Wall vault
		if(members->verticalRunning)
		{
			State_RunnerController_WallVault(obj, state);
		}

		members->verticalRunning = members->horizontalRunning = 0;

	}


	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	if(members->spinningTimer < members->spinningTime)
	{
		float dt = TimeManager_GetDeltaSec();
		members->spinningTimer += dt;
		Camera_ChangeYaw(cam, spinningRate * dt);
	}
	else
	{
		//Rotate runner
		State_RunnerController_Rotate(obj, state);
	}
	//Grab the camera

	// Set position of Camera to the body
	Camera_SetPosition(cam, obj->body->frame->position);
}



///
//Accelerates the runner controller
//
//PArameters:
//	obj: A pointer to the game object to accelerate
//	state: A pointer to rhe runner controller state which is accelerating this object
void State_RunnerController_Accelerate(GObject* obj, State* state)
{
	//Grab the state members
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;

	//Grab the forward vector from the camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	Vector forward;
	Vector_INIT_ON_STACK(forward, 3);

	Matrix_SliceRow(&forward, cam->rotationMatrix, 2, 0, 3);

	//Project the forward vector onto the XY Plane
	Vector perp;
	Vector_INIT_ON_STACK(perp, 3);

	Vector_GetProjection(&perp, &forward, &Vector_E2);
	Vector_Decrement(&forward, &perp);

	//Scale the vector to the acceleration
	Vector_Normalize(&forward);
	Vector_Scale(&forward, -members->acceleration);



	//Only apply the impulse if the velocity is less than the max speed
	if(Vector_GetMag(obj->body->velocity) - fabs(Vector_DotProduct(obj->body->velocity, &Vector_E2)) < members->maxVelocity)
	{
		//Apply the impulse
		RigidBody_ApplyForce(obj->body, &forward, &Vector_ZERO);
	}
	else
	{
		printf("Value:\t%f\n", Vector_GetMag(obj->body->velocity) - fabs(Vector_DotProduct(obj->body->velocity, &Vector_E2)));
	}
}

///
//Rotates the runner controller
//	obj: A pointer to the game object to rotate
//	state: A pointer to the runner controller rotating the object
void State_RunnerController_Rotate(GObject* obj, State* state)
{
	//obj is unused
	(void)obj;

	// create a camera object
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	//Grab the state members
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;

	// if player's mouse is locked
	if(InputManager_GetInputBuffer().mouseLock)
	{

		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		Vector* axis = Vector_Allocate();
		Vector_Initialize(axis,3);

		if(deltaMouseX != 0)
		{
			axis->components[1] = 1.0f;
			// rotate the camera
			Camera_ChangeYaw(cam, members->angularVelocity * deltaMouseX);
			axis->components[1] = 0.0f;
		}

		if (deltaMouseY != 0)
		{
			Vector forwardVector;
			Vector_INIT_ON_STACK(forwardVector, 3);
			Matrix_SliceRow(&forwardVector, cam->rotationMatrix, 2, 0, 3);

			// Keep camera from overextending it's boundaries.
			if (deltaMouseY > 0)
			{
				if (Vector_DotProduct(&forwardVector, &Vector_E2) < 0.7f)
				{
					axis->components[0] = 1.0f;
					Camera_ChangePitch(cam, members->angularVelocity * deltaMouseY);
					axis->components[0] = 0.0f;
				}
			}
			else if (deltaMouseY < 0)
			{
				if (Vector_DotProduct(&forwardVector, &Vector_E2) > -0.7f)
				{
					axis->components[0] = 1.0f;
					Camera_ChangePitch(cam, members->angularVelocity * deltaMouseY);
					axis->components[0] = 0.0f;
				}
			}
		}
	}
}

///
//Allows the runner controller to jump if necessary conditions are met
//
//Parameters:
//	obj: A pointer to the object jumping
//	state: A pointer to the runner controller state which is jumping the object
void State_RunnerController_Jump(GObject* obj, State* state)
{

	//If the object is allowed to jump
	if(State_RunnerController_IsOnGround(obj, state))
	{
		//Grab the state members
		struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;


		Vector jumpImpulse;
		Vector_INIT_ON_STACK(jumpImpulse, 3);

		jumpImpulse.components[1] = members->jumpMag;

		RigidBody_ApplyImpulse(obj->body, &jumpImpulse, &Vector_ZERO);
	}
}

///
//Allows the runner controller to wallrun if necessary conditions are met
//
//Parameters:
//	obj: A pointer to the object which is running on walls
//	state: A pointer to the runner controller state which is allowing the object to wallrun
void State_RunnerController_Wallrun(GObject* obj, State* state)
{
	

	//Get the members of this state
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;


	//Get the first collision this object is involved in
	struct Collision* first = (struct Collision*)obj->collider->currentCollisions->head->data;

	//If we are not wallrunning yet
	if(members->horizontalRunning == 0 && members->verticalRunning == 0)
	{
		//Make sure this is a wall
		if(first->minimumTranslationVector->components[0] != 0.0 || first->minimumTranslationVector->components[2] != 0.0f)
		{
			//Save the normal
			Vector_Copy(members->wallNormal, first->minimumTranslationVector);

			if(first->obj1 != obj)
			{
				Vector_Scale(members->wallNormal, -1.0f);
			}

			//Determine what kind of wallrun is occurring
			//First get the forward vector of the camera
			Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

			Vector forward;
			Vector_INIT_ON_STACK(forward, 3);

			Matrix_SliceRow(&forward, cam->rotationMatrix, 2, 0, 3);

			//Project the forward vector onto the XY Plane
			Vector perp;
			Vector_INIT_ON_STACK(perp, 3);

			Vector_GetProjection(&perp, &forward, &Vector_E2);
			Vector_Decrement(&forward, &perp);

			Vector_Normalize(&forward);

			//Get dot product of forward vector and collision normal
			float dotProd = fabs(Vector_DotProduct(&forward, first->minimumTranslationVector));
			//If the dot product is closer to 0 we are horizontal running, else we are vertical running
			if(dotProd < 0.75)
			{
				members->horizontalRunning = 1;
			}
			else
			{
				members->verticalRunning = 1;

			}
		}
	}

	//If we are horizontal running
	if(members->horizontalRunning == 1)
	{
		printf("Horizontal Wallrunnin\n");

		//combat the force of gravity
		Vector antiGravity;
		Vector_INIT_ON_STACK(antiGravity, 3);
		antiGravity.components[1] = 9.81f;
		RigidBody_ApplyForce(obj->body, &antiGravity, &Vector_ZERO);

		//Zero downward velocity
		if(obj->body->velocity->components[1] < 0.0f)
		{
			Vector_Copy(&antiGravity, &Vector_ZERO);
			antiGravity.components[1] = -obj->body->velocity->components[1];
			RigidBody_ApplyImpulse(obj->body, &antiGravity, &Vector_ZERO);
		}


		State_RunnerController_Accelerate(obj, state);


	}
	else if(members->verticalRunning == 1)
	{
		printf("Vertical Wallrunnin\n");


		//combat the force of gravity
		Vector antiGravity;
		Vector_INIT_ON_STACK(antiGravity, 3);
		Vector_Copy(&antiGravity, &Vector_E2);


		//go up!
		Vector_Scale(&antiGravity, 9.81);
		RigidBody_ApplyForce(obj->body, &antiGravity, &Vector_ZERO);

		//If we aren't jumping too fast yet
		if(Vector_DotProduct(obj->body->velocity, &Vector_E2) < members->maxVelocity)
		{
			Vector_Copy(&antiGravity, &Vector_E2);
			Vector_Scale(&antiGravity, members->acceleration);
			RigidBody_ApplyForce(obj->body, &antiGravity, &Vector_ZERO);
		}
	}
}

///
//Lets the runner jump off of a wall
//	obj: A pointer to the object
//	state: A pointer to the runner controller state which is allowing the object to wall jump
void State_RunnerController_WallJump(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;

	Vector impulse;
	Vector_INIT_ON_STACK(impulse, 3);

	Vector_Copy(&impulse, members->wallNormal);
	Vector_Scale(&impulse, members->jumpMag * 2);

	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);

	Vector_Copy(&impulse, &Vector_E2);
	Vector_Scale(&impulse, members->jumpMag);
	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);
}


///
//Lets the runner vault off of a wall
//	obj: A pointer to the object
//	state: A pointer to the runner controller state which is allowing the object to wall vault
void State_RunnerController_WallVault(GObject* obj, State* state)
{
	//Get the members of this state
	struct State_RunnerController_Members* members = (struct State_RunnerController_Members*)state->members;

	Vector impulse;
	Vector_INIT_ON_STACK(impulse, 3);

	Vector_Copy(&impulse, members->wallNormal);


	float mag = obj->body->velocity->components[1];

	Vector_Scale(&impulse, -mag * 1.0f);
	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);

	Vector_Copy(&impulse, &Vector_E2);
	obj->body->velocity->components[1] = 0;

	Vector_Scale(&impulse, members->jumpMag);
	RigidBody_ApplyImpulse(obj->body, &impulse, &Vector_ZERO);

}
