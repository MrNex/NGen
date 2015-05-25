#include "ApplyForce.h"

struct State_Force_Members
{
	Vector* force;
	Vector* radius;
};

///
//Initializes a new force state
//
//Parameters:
//	state: A pointer to the state to initialize as a force state
//	force: A pointer to a vector to copy as the force this state will apply
//	radius: A pointer to a vector to copy as the radius this state will apply the force at
void State_Force_Initialize(State* state, Vector* force, Vector* radius)
{
	struct State_Force_Members* members = (struct State_Force_Members*)malloc(sizeof(struct State_Force_Members));
	state->members = (State_Members)members;

	members->force = Vector_Allocate();
	Vector_Initialize(members->force, 3);
	Vector_Copy(members->force, force);

	members->radius = Vector_Allocate();
	Vector_Initialize(members->radius, 3);
	Vector_Copy(members->radius, radius);

	state->State_Members_Free = State_Force_Free;
	state->State_Update = State_Force_Update;
}

///
//Frees resources allocated by a force state
//
//Parameters:
//	state: A pointer to the state to free
void State_Force_Free(State* state)
{
	struct State_Force_Members* members = (struct State_Force_Members*)state->members;
	Vector_Free(members->force);
	Vector_Free(members->radius);

	free(members);
}

///
//Updates a force state by applying the defined force to the GObject
//
//Parameters:
//	GO: A pointer to the game object to apply the force to
//	state: A pointer to the state updating the attached object
void State_Force_Update(GObject* GO, State* state)
{
	struct State_Force_Members* members = (struct State_Force_Members*)state->members;

	RigidBody_ApplyForce(GO->body, members->force, members->radius);
}
