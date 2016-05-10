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


#include "Remove.h"


#include "../Manager/TimeManager.h"
#include "../Manager/ObjectManager.h"

struct State_Remove_Members
{
	float currentTime;
	float removeTime;
	unsigned int objID;
};

///
//Initializes a remove state
//
//Parameters:
//	s: The state to initialize as a remove state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Remove_Initialize(State* state, float seconds, unsigned int objID)
{
	state->members = (State_Members)malloc(sizeof(struct State_Remove_Members));
	//Get members
	struct State_Remove_Members* members = (struct State_Remove_Members*)state->members;

	members->currentTime = 0.0f;
	members->removeTime = seconds;
	members->objID = objID;

	state->State_Members_Free = State_Remove_Free;
	state->State_Update = State_Remove_Update;
}

///
//Frees memory allocated by a remove state
//
//Parameters:
//	state: The state to free
void State_Remove_Free(State* state)
{
	//Get members
	struct State_Remove_Members* members = (struct State_Remove_Members*)state->members;

	free(members);
}

///
//Updates a remove state removing the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached remove state
//	state: THe state being updated
void State_Remove_Update(GObject* GO, State* state)
{
	//Get members
	struct State_Remove_Members* members = (struct State_Remove_Members*)state->members;

	members->currentTime += TimeManager_GetDeltaSec();
	if(members->currentTime > members->removeTime)
	{
		ObjectManager_QueueReleaseObject(members->objID);
	}
}
