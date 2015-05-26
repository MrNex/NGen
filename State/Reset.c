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


#include "Reset.h"

#include <stdio.h>

#include "../Manager/TimeManager.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/CollisionManager.h"

struct State_Reset_Members
{
	float currentTime;
	float resetTime;
	float resetDistance;
	Vector* initialPosition;
	Vector* initialImpulse;
	Matrix* initialRotation;
};

///
//Initializes a Reset state
//
//Parameters:
//	s: The state to initialize as a Reset state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Reset_Initialize(State* state, float seconds, float distance, Vector* initPos, Vector* initImp, Matrix* initRot)
{
	state->members = (State_Members)malloc(sizeof(struct State_Reset_Members));
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;

	members->resetTime = seconds;
	members->currentTime = (members->resetTime + 1.0f);

	members->resetDistance = distance;

	members->initialPosition = Vector_Allocate();
	Vector_Initialize(members->initialPosition, 3);
	members->initialImpulse = Vector_Allocate();
	Vector_Initialize(members->initialImpulse, 3);


	Vector_Copy(members->initialPosition, initPos);
	Vector_Copy(members->initialImpulse, initImp);

	members->initialRotation = Matrix_Allocate();
	Matrix_Initialize(members->initialRotation, 3, 3);

	Matrix_Copy(members->initialRotation, initRot);

	state->State_Members_Free = State_Reset_Free;
	state->State_Update = State_Reset_Update;
}

///
//Frees memory allocated by a Reset state
//
//Parameters:
//	state: The state to free
void State_Reset_Free(State* state)
{
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;
	Vector_Free(members->initialPosition);
	Vector_Free(members->initialImpulse);
	Matrix_Free(members->initialRotation);

	free(members);
}

///
//Updates a Reset state removing the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached Reset state
//	state: THe state being updated
void State_Reset_Update(GObject* GO, State* state)
{
	//Get members
	struct State_Reset_Members* members = (struct State_Reset_Members*)state->members;

	if(GO->collider->currentCollisions->size > 0 && members->currentTime > members->resetTime)
	{

		//Loop through the collisions which occurred previous frame
		struct LinkedList_Node* current = GO->collider->currentCollisions->head;
		struct Collision* currentCollision;
		while(current != NULL)
		{
			//Check if any of the objects involved in the collision are a bullet
			//TODO: MAke a tagging system so this doesn't need to happen
			currentCollision = (struct Collision*)current->data;
			//Bullets are the only thing with a scale of 0.3
			if(currentCollision->obj1->frameOfReference->scale->components[0] == 0.9f || currentCollision->obj2->frameOfReference->scale->components[0] == 0.9f)
			{
				members->currentTime = 0.0f;
				break;
			}
			current = current->next;

		}
	}
	else if(members->currentTime < members->resetTime)
	{
		members->currentTime += TimeManager_GetDeltaSec();
		if(members->currentTime > members->resetTime)
		{
			if(GO->body != NULL)
			{
				Vector_Copy(GO->body->velocity, &Vector_ZERO);
				Vector_Copy(GO->body->angularVelocity, &Vector_ZERO);
				RigidBody_ApplyImpulse(GO->body, members->initialImpulse, &Vector_ZERO);
			}
			GObject_SetPosition(GO, members->initialPosition);

			GObject_SetRotation(GO, members->initialRotation);
		}
	}

}
