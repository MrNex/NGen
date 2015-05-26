#if defined __linux__
#define linux

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif


#include "Score.h"
#include <stdio.h>

#include "../Manager/CollisionManager.h"
#include "../Manager/TimeManager.h"

struct State_Score_Members
{
	int worth;
	float timerCap;
	float timer;
};

///
//Initializes a new score state
//
//Parameters:
//	state: The state to initialize as a score state
//	increment: The amount of points to add per hit
//	timePerIncrement: The amount of seconds you must wait before scoring on this target again
void State_Score_Initialize(State* state, const int increment, const float timePerIncrement)
{
	struct State_Score_Members* members = (struct State_Score_Members*)malloc(sizeof(struct State_Score_Members));
	state->members = members;

	members->worth = increment;
	members->timerCap = timePerIncrement;
	members->timer = 0.0f;

	state->State_Members_Free = State_Score_Free;
	state->State_Update = State_Score_Update;
}

///
//Frees resources allocated by a score state
//
//Parameters:
//	state: A pointer to the state to free
void State_Score_Free(State* state)
{
	struct State_Score_Members* members = (struct State_Score_Members*)state->members;
	free(members);
}

///
//Checks if the attached object was in a collision with a bullet, then increments & prints the score if so
//
//parameters:
//	GO: a pointer to The game object with an attached score state being updated
//	state: a pointer to the score state updating the attached game object
void State_Score_Update(GObject* GO, State* state)
{
	struct State_Score_Members* members = (struct State_Score_Members*)state->members;

	if(members->timer > members->timerCap)
	{

		if(GO->collider->currentCollisions->size > 0)
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
					score += members->worth;
					//Print the new score
					printf("Score:\t%d\n", score);
					members->timer = 0.0f;
				}
				current = current->next;
			}
		}
	}
	else
	{
		members->timer += TimeManager_GetDeltaSec();
	}
}
