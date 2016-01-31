#include "ProximityRemove.h"

#include "../Manager/ObjectManager.h"

struct State_ProximityRemove_Members
{
	Vector* target;
	float distance;
};


///
//Initializes a Proxiimityremove state
//
//Parameters:
//	s: A pointer to the state to initialize as a proximity remove state
//	target: A pointer to the vector to track the distance this game object is away from
//	dist: The maximum distance between attached GObject before deletion
void State_ProximityRemove_Initialize(State* s, Vector* target, float dist)
{
	struct State_ProximityRemove_Members* members = malloc(sizeof(struct State_ProximityRemove_Members));
	s->members = members;

	members->target = target;
	members->distance = dist;

	s->State_Update = State_ProximityRemove_Update;
	s->State_Members_Free = State_ProximityRemove_Free;
}

///
//Updates a GObject using a ProximityRemove state
//the state will delete the attached object once it is further away
//than the specified distance from the target
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the proximityRemove state updating the GObject
void State_ProximityRemove_Update(GObject* obj, State* state)
{
	struct State_ProximityRemove_Members* members = (struct State_ProximityRemove_Members*)state->members;

	Vector toTarget;
	Vector_INIT_ON_STACK(toTarget, 3);

	Vector_Subtract(&toTarget, members->target, obj->frameOfReference->position);

	if(Vector_GetMag(&toTarget) > members->distance)
	{
		printf("Deleting at distance %f\n\n", Vector_GetMag(&toTarget));

		ObjectManager_QueueDelete(obj);
	}
}

///
//Frees memory allocated by a ProximityRemove state
//
//Parameters:
//	state: The Template state to free
void State_ProximityRemove_Free(State* state)
{
	free(state->members);
}
