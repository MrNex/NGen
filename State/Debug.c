#include "Debug.h"
#include <string.h>

struct State_Debug_Members
{
	char* name;
};


///
//Initializes a Debug state
//
//Parameters:
//	s: A pointer to the state to initialize
//	name: the name to print with the given debug statement
void State_Debug_Initialize(State* s, char* name)
{
	struct State_Debug_Members* members = (struct State_Debug_Members*)malloc(sizeof(struct State_Debug_Members));
	s->members = members;

	members->name = (char*)malloc(sizeof(char) * strlen(name));
	strcpy(members->name, name);

	s->State_Update = State_Debug_Update;
	s->State_Members_Free = State_Debug_Free;	
}

///
//Updates a GObject using a Debug state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Debug_Update(GObject* obj, State* state)
{

	struct State_Debug_Members* members = (struct State_Debug_Members*)state->members;
	RigidBody* body = obj->body;
	if(obj->collider->currentCollisions->size > 2)
	{
	printf("%s:\t", members->name);
	Vector_PrintTranspose(body->angularVelocity);
	}
}

///
//Frees memory allocated by a Debug state
//
//Parameters:
//	state: The Debug state to free
void State_Debug_Free(State* state)
{
	struct State_Debug_Members* members = (struct State_Debug_Members*)state->members;
	free(members->name);
	free(members);
}
