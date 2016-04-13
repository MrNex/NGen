#include "HorizontalController.h"

#include "../Manager/InputManager.h"

struct State_HorizontalController_Members
{
	float movementSpeed;
};

///
//Initializes a HorizontalController state
//
//Parameters:
//	s: A pointer to the state to initialize
//	speed: the speed of the movement
void State_HorizontalController_Initialize(State* s, float speed)
{
	struct State_HorizontalController_Members* members = malloc(sizeof(struct State_HorizontalController_Members));
	s->members = members;

	members->movementSpeed = speed;

	s->State_Members_Free = State_HorizontalController_Free;
	s->State_Update = State_HorizontalController_Update;
}

///
//Updates a GObject using a HorizontalController state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_HorizontalController_Update(GObject* obj, State* state)
{

	struct State_HorizontalController_Members* members = state->members;
	Vector mvmt;
	Vector_INIT_ON_STACK(mvmt, 3);

	if(InputManager_IsKeyDown('i'))
	{
		mvmt.components[2] = -members->movementSpeed;
	}
	if(InputManager_IsKeyDown('k'))
	{
		mvmt.components[2] = members->movementSpeed;
	}
	if(InputManager_IsKeyDown('j'))
	{
		mvmt.components[0] = -members->movementSpeed;
	}
	if(InputManager_IsKeyDown('l'))
	{
		mvmt.components[0] = members->movementSpeed;
	}

	GObject_Translate(obj, &mvmt);

}

///
//Frees memory allocated by a HorizontalController state
//
//Parameters:
//	state: The HorizontalController state to free
void State_HorizontalController_Free(State* state)
{
	free(state->members);
}
